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

void Evaluater::check_array(
  long depth,
  std::vector<Node*>& uninitialized_list,
  typename std::vector<Node*>::const_iterator elemcount_it,
  Node* array
  ) {
  auto cur_var_s = *var_stmt_list.begin();

  if( depth == 0 )
    return;

  if( *elemcount_it != nullptr ) {
    assert(array->is_allowed_empty_array);

    if( array->kind != NODE_ARRAY ) {
      error(ERR_TYPE, array->token, "expected empty array");
    }
    else if( !array->list.empty() ) {
      error(ERR_TYPE, array->token, "array is must empty");
    }
    else { // it is array, and empty
      array->elemcount = &cur_var_s->objects.emplace_back();
    }
  }

  for( auto&& i : array->list ) {
    check_array(depth - 1, uninitialized_list, elemcount_it + 1, i);
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

    case NODE_SCOPE: {
      Vec v;

      for( auto it = node->list.rbegin(); it != node->list.rend(); it++ ) {
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

std::pair<bool, Node*> Evaluater::is_integrated(Node* node) {
  // dont use

  ObjectType type;
  auto&& vec = get_return_values(node);

  for( auto it = vec.begin(); it != vec.end(); it++ ) {
    if( it == vec.begin() ) {
      type = evaluate(*it);
      continue;
    }
    
    if( !type.equals(evaluate(*it)) ) {
      return { false, *it };
    }
  }

  return { true, nullptr };
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
      error(ERR_TYPE, node->token, "all types of return value is not integrated.");
      error(ERR_NOTE, types[0]->token, "was inferred as '%s' here", firststr.c_str());
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

Node* Evaluater::find_func(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
    for( auto&& i : (*it)->list ) {
      if( i->kind == NODE_FUNCTION && i->name == name ) {
        return i;
      }
    }
  }

  return nullptr;
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

      for( auto&& i : node->list ) {
        evaluate(i);
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

      throw 0;
      // todo: user defined

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
      auto func_type = evaluate(node->type);
      auto eval = evaluate(node->expr);

      auto list = get_return_values(node->expr);
      auto err = false;

      for( auto&& i : list ) {
        auto&& e = evaluate(i);

        if( !func_type.equals(e) ) {
          if( !err ) {
            std::cout << Global::get_instance()->file_path << ": in function '" << node->name << "'" << std::endl;
            err = true;
          }

          error(ERR_TYPE, i->token, "expected '%s' , but found '%s'", func_type.to_string().c_str(), e.to_string().c_str());
          break;
        }
      }

      if( err ) {
        error(ERR_NOTE, node->type->token, "specified here");
      }

      break;
    }

    case NODE_SCOPE: {
      if( node->list.empty() ) {
        break;
      }
      
      scope_list.push_front(node);
      
      for( auto&& i : node->list ) {
        if( !i ) {
          continue;
        }
        
        i->is_allowed_let = true;
        evaluate(i);
      }

      if( node != Global::get_instance()->top_node ) {
        ret = must_integrated(node);
      }

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
        obj.type = specified_type;
      }

      // have initializer expr
      if( node->expr ) {
        if( specified_type.arr_depth ) {
          node->expr->is_allowed_empty_array = true;
          (node->expr->objtype = specified_type).arr_depth = 1;
        }

        auto expr_type = evaluate(node->expr);

        if( node->type ) {
          // not matching types which specify and initializer
          if( !expr_type.equals(specified_type) ) {
            std::cout << specified_type << ", " << expr_type << std::endl;
            error(ERR_TYPE, node->token, "type mismatch");
            var_stmt_list.pop_front();
            break;
          }
        }
        else {
          obj.type = expr_type;
        }

        if( node->type && specified_type.arr_depth ) {
          node->is_make_array = true;
          check_array(specified_type.arr_depth, node->list, node->type->elemcount_list.cbegin(), node->expr);
        }
      }

      var_stmt_list.pop_front();
      break;
    }

    case NODE_IF: {
      evaluate(node->expr);

      ret = evaluate(node->if_true);

      if( node->if_else ) {
        if( !ret.equals(evaluate(node->if_else)) ) {
          error(ERR_TYPE, node->token, "type mismatch");
        }
      }

      break;
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