#include "chang.h"

static Evaluater* _instance;

Evaluater::Evaluater() {
  _instance = this;
}

Evaluater* Evaluater::get_instance() {
  return _instance;
}

bool Evaluater::is_branchable(Node* node) {
  if( !node )
    return false;

  switch( node->kind ) {
    case NODE_IF:
      return true;
  }

  return false;
}

void Evaluater::check_array(std::vector<Node*>::const_iterator ec_list_it, std::vector<Object>::const_iterator ec_obj_list_it, std::size_t depth, Node* arr) {
  auto arr_type = evaluate(arr);
  arr->elemcount = &*ec_obj_list_it;

  if( *ec_list_it != nullptr ) {
    if( arr->kind != NODE_ARRAY ) {
      error(ERR_TYPE, arr->token, "expected empty array");
      return;
    }
    else if( !arr->list.empty() ) {
      error(ERR_TYPE, arr->token, "array must be empty");
      return;
    }

    arr->objtype.arr_depth = depth;
    return;
  }

  if( arr_type.arr_depth != depth ) {
    error(ERR_TYPE, arr->token, "type mismatch");
  }

  if( depth >= 2 && arr->kind == NODE_ARRAY ) {
    for( auto&& i : arr->list ) {
      check_array(ec_list_it + 1, ec_obj_list_it + 1, depth - 1, i);
    }
  }
}

std::vector<Node*> Evaluater::get_return_values(Node* node) {
  using Vec = std::vector<Node*>;

  if( !node )
    return { };

  switch( node->kind ) {
    case NODE_IF: {
      Vec v;

      for( auto&& i : get_return_values(node->if_true) ) {
        v.emplace_back(i);
      }

      for( auto&& i : get_return_values(node->if_else) ) {
        v.emplace_back(i);
      }

      return v;
    }

    case NODE_RETURN: {
      return { node->expr };
    }

    case NODE_SCOPE: {
      Vec v;
      Vec::const_iterator begin = node->list.end() - 1;

      for( auto it = node->list.begin(); it != node->list.end(); it++ ) {
        if( *it && (*it)->kind == NODE_RETURN ) {
          begin = it;
          break;
        }
      }

      for( auto it = begin; it >= node->list.begin(); it-- ) {
        for( auto&& i : get_return_values(*it) ) {
          v.emplace_back(i);
        }

        if( !is_branchable(*it) ) {
          break;
        }
        else { // keep this new line
          if( (*it)->kind == NODE_IF && (*it)->if_else && (*it)->if_else->kind != NODE_IF ) {
            break;
          }
        }
      }

      return v;
    }

    case NODE_FUNCTION: {
      return get_return_values(node->expr);
    }

    case NODE_VAR:
      return { };
  }

  return { node };
}

ObjectType Evaluater::must_integrated(Node* node) {
  assert(node->kind == NODE_SCOPE);

  auto types = get_return_values(node);

  if( types.empty() ) {
    return { };
  }

  auto const first = evaluate(types[0]);
  auto const&& firststr = first.to_string();

  if( types.size() <= 1 ) {
    return first;
  }

  for( auto it = types.begin() + 1; it != types.end(); it++ ) {
    auto&& eval = evaluate(*it);

    if( !first.equals(eval) ) {
      error(ERR_TYPE, node->token, "all values which can be return value of scope are must be integrated by one type.");
      error(ERR_NOTE, types[0]->token, "return type was inferred as '%s' here", firststr.c_str());
      error(ERR_TYPE, (*it)->token, "expected '%s', but found '%s'", firststr.c_str(), eval.to_string().c_str());
      exit(1);
    }
  }

  return first;
}

std::pair<Node*, std::size_t> Evaluater::find_var(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
    auto find = (*it)->find_var(name);

    if( find != -1 ) {
      return { *it, find };
    }
  }
  
  return { nullptr, 0 };
}

std::vector<ObjectType> Evaluater::eval_func_args(Node* func) {
  std::vector<ObjectType> ret;

  for( auto&& i : func->list ) {
    ret.emplace_back(evaluate(i->type));
  }

  return ret;
}

std::vector<Node*> Evaluater::find_func(std::string_view const& name, std::vector<ObjectType> const& arg_types) {
  std::vector<Node*> ret;

  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
    for( auto&& i : (*it)->list ) {
      if( i && i->kind == NODE_FUNCTION && i->name == name ) {
        auto args = eval_func_args(i);

        if( args.size() != arg_types.size() ) {
          continue;
        }
        else {
          for( std::size_t i = 0; i < args.size(); i++ ) {
            if( !args[i].equals(arg_types[i]) ) {
              goto cnt_label;
            }
          }
        }

        ret.emplace_back(i);
      cnt_label:;
      }
    }
  }

  return ret;
}

ObjectType Evaluater::evaluate(Node* node) {
  if( !node )
    return { };

  if( node->evaluated )
    return node->objtype;

  auto& ret = node->objtype;
  node->evaluated = true;

  switch( node->kind ) {
    case NODE_VALUE:
      alert;
      ret = node->obj.type;
  

      break;
    
    case NODE_VARIABLE: {
      auto [scope, index] = find_var(node->name);

      if( scope ) {
        node->var_scope = scope;
        node->var_index = index;
        ret = scope->objects[index].type;
        break;
      }

      error(ERR_UNDEFINED, node->token, "undefined variable name '%s'", Utils::str(node->name));
      exit(1);
    }
    
    case NODE_ARRAY: {
      if( node->is_allowed_empty_array ) {
        if( node->list.empty() ) {
          return node->objtype;
        }
        
        for( auto&& i : node->list ) {
          i->is_allowed_empty_array = true;
          i->objtype = node->objtype;
        }
      }
      else if( node->list.empty() ) {
        error(ERR_TYPE, node->token, "empty array is invalid");
        exit(1);
      }

      auto first = evaluate(node->list[0]);

      for( auto it = node->list.begin() + 1; it != node->list.end(); it++ ) {
        if( !first.equals(evaluate(*it)) ) {
          error(ERR_TYPE, (*it)->token, "type mismatch");
        }
      }

      first.arr_depth += 1;
      return ret = first;
    }

    case NODE_CALLFUNC: {
      auto const& builtin = BuiltinFunc::get_list();

      std::size_t argc;
      bool arg_free;
      std::vector<ObjectType> args;
      std::vector<Node*> find_userdef;
      Node* fn;

      for( auto&& i : node->list ) {
        args.emplace_back(evaluate(i));
      }

      for( auto&& i : builtin ) {
        if( i.name == node->name ) {
          argc = i.arg_types.size();
          arg_free = i.arg_free;
          node->builtin = &i;
          ret = i.ret_type;
          goto check_process;
        }
      }

      find_userdef = find_func(node->name, args);

      if( find_userdef.empty() ) {
        error(ERR_UNDEFINED, node->token, "undefined function");

        for( auto&& scope : scope_list ) {
          for( auto&& i : scope->list ) {
            if( i && i->kind == NODE_FUNCTION && i->name == node->name )
              find_userdef.emplace_back(i);
          }
        }

        if( !find_userdef.empty() ) {
          error(ERR_NOTE, node->token, "but found some function with same name, maybe did you tried call with wrong arguments?");

          for( auto&& i : find_userdef ) {
            error(ERR_NOTE, i->token, "got this");
          }
        }
      }
      else if( find_userdef.size() > 1 ) {
        error(ERR_MANY_CANDIDATES, node->token, "found many candidates with this name");

        for( auto&& i : find_userdef ) {
          error(ERR_NOTE, i->token, "got this");
        }
      }

      fn = find_userdef[0];

      ret = evaluate(fn->expr);
      node->func = fn;

      break;

    check_process:;
      if( !arg_free ) {
        if( node->list.size() < argc ) {
          error(ERR_ARGUMENT, node->token, "too few argument");
        }
        else if( node->list.size() > argc ) {
          error(ERR_ARGUMENT, node->token, "too many argument");
        }
      }

      break;
    }

    case NODE_TYPE: {
      if( node->name == "int" )
        ret = OBJ_INT;
      else if( node->name == "char" )
        ret = OBJ_CHAR;
      else if( node->name == "float" )
        ret = OBJ_FLOAT;
      else if( node->name == "bool" )
        ret = OBJ_BOOL;
      else if( node->name == "string" )
        ret = OBJ_STRING;
      else if( node->name == "none" )
        ret = OBJ_NONE;
      else {
        // todo: find struct

        error(ERR_TYPE, node->token, "unknown type name");
        exit(1);
      }

      for( auto&& i : node->elemcount_list ) {
        if( i && !evaluate(i).equals(OBJ_INT) ) {
          error(ERR_TYPE, i->token, "elements count is must be integer");
        }

        ret.arr_depth++;
      }

      break;
    }

    case NODE_ARGUMENT: {
      ret = evaluate(node->type);
      break;
    }

    case NODE_FUNCTION: {
      for( auto&& arg : node->list ) {
        auto& obj = node->expr->objects.emplace_back();

        obj.type = evaluate(arg->type);
        obj.name = arg->name;
      }

      auto func_type = evaluate(node->type);
      auto eval = evaluate(node->expr);

      auto list = get_return_values(node->expr);
      auto err = false;

      if( list.empty() && !func_type.equals(OBJ_NONE) ) {
        error(ERR_TYPE, node->token, "return type is not none, but function will return nothing.");
      }

      for( auto&& i : list ) {
        auto&& e = evaluate(i);

        if( !func_type.equals(e) ) {
          if( !err ) {
            std::cout << Global::get_instance()->file_path << ": In function '" << node->name << "'" << std::endl;
            err = true;
          }

          error(ERR_TYPE, i->token, "expected '%s', but found '%s'", func_type.to_string().c_str(), e.to_string().c_str());
        }
      }

      if( err ) {
        error(ERR_NOTE, node->type ? node->type->token : node->token, "specified here");
      }

      break;
    }

    case NODE_SCOPE: {
      if( node->list.empty() ) {
        break;
      }
      
      scope_list.push_front(node);

      auto returned = false;
      Node* ret_nd;
      
      for( auto&& i : node->list ) {
        // if( !i ) {
        //   continue;
        // }

    if(i){
        if( returned ) {
          error(ERR_RETURN, i->token, "code is invalid after return statement");
          error(ERR_NOTE, ret_nd->token, "returned here");
        }
        else if( i->kind == NODE_RETURN ) {
          returned = true;
          ret_nd = i;
        }
        
        alert;
        i->is_allowed_let = true;
        i->is_allowed_return = true;
        
        alert;
        evaluate(i);
    }
      }

      alert;
      if( node != Global::get_instance()->top_node ) {
        ret = must_integrated(node);
      }

      alert;
      scope_list.pop_front();
      break;
    }

    case NODE_VAR: {
      var_stmt_list.push_front(node);

      if( !node->is_allowed_let ) {
        error(ERR_LOCATION, node->token, "cannot declare variable here");
        exit(1);
      }

      auto [scope, index] = find_var(node->name);

      if( scope == *scope_list.begin() ) {
        error(ERR_MULTIPLE_DEFINED, node->token, "multiple defined variable name");
        exit(1);
      }

      auto cur = *scope_list.begin();
      auto& obj = cur->objects.emplace_back();

      obj.name = node->name;

      node->var_scope = cur;
      node->var_index = cur->objects.size() - 1;

      auto specified_type = evaluate(node->type);

      if( node->type ) {
        auto chk_type = node->expr != nullptr;
        obj.type = specified_type;

        if( specified_type.arr_depth ) {
          auto flag = false;
          
          node->is_make_array = true;

          for( auto it = node->type->elemcount_list.begin(); it != node->type->elemcount_list.end(); it++ ) {
            if( *it == nullptr ) {
              if( !flag ) {
                flag = true;
              }
            }
            else if( flag ) {
              error(ERR_TYPE, (*it)->token, "cannot specify elements count of array in this depth, due to not specified previous depth.");
              error(ERR_NOTE, (*it)->token->back->back->back, "due to elements will be empty in this depth, the next count specification is invalid.");
              return { };
            }

            node->objects.emplace_back();
          }

          if( node->type->elemcount_list[0] ) {
            if( node->expr ) {
              node->expr->is_allowed_empty_array = true;
              (node->expr->objtype = specified_type).arr_depth = 1;
              check_array(node->type->elemcount_list.begin(), node->objects.begin(), specified_type.arr_depth, node->expr);
            }

            var_stmt_list.pop_front();
            break;
          }
        }

        if( chk_type && !evaluate(node->expr).equals(specified_type) ) {
          error(ERR_TYPE, node->token, "type mismatch");
        }
      }
      else {
        obj.type = evaluate(node->expr);
      }

      var_stmt_list.pop_front();
      break;
    }

    case NODE_IF: {
      if( !evaluate(node->expr).equals(OBJ_BOOL) ) {
        error(ERR_TYPE, node->token, "condition is must boolean");
      }

      ret = evaluate(node->if_true);

      if( node->if_else ) {
        if( !ret.equals(evaluate(node->if_else)) ) {
          error(ERR_TYPE, node->token, "type mismatch");
        }
      }

      break;
    }

    case NODE_RETURN: {
      if( !node->is_allowed_return ) {
        error(ERR_LOCATION, node->token, "cannot use return here");
      }

      return evaluate(node->expr);
    }

    case NODE_EXPR: {
      ret = evaluate(node->expr);

      for( auto&& item : node->expr_list ) {
        if( !ret.equals(evaluate(item.item)) ) {
          error(ERR_TYPE, item.token, "type mismatch");
        }
      }

      break;
    }
  }
  
  return ret;
}