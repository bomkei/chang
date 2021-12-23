#include "chang.h"

bool Evaluater::is_branchable(Node* node) {
  if( !node )
    return false;

  switch( node->kind ) {
    case NODE_IF:
      return true;
  }

  return false;
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
        else {
          if( (*it)->kind == NODE_IF && (*it)->if_else ) {
            break;
          }
        }
      }

      return v;
    }

    case NODE_FUNCTION: {
      return get_return_values(node->expr);
    }
  }

  return { node };
}

std::pair<bool, Node*> Evaluater::is_integrated(Node* node) {
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

void Evaluater::must_integrated(Node* node) {
  assert(node->kind == NODE_SCOPE);

  if( node->list.size() <= 1 )
    return;
  
  auto types = get_return_values(node);

  if( types.empty() )
    return;

  auto const first = evaluate(types[0]);
  auto const firststr = first.to_string();

  if( types.size() <= 1 )
    return;

  for( auto it = types.begin() + 1; it != types.end(); it++ ) {
    auto&& eval = evaluate(*it);

    if( !first.equals(eval) ) {
      error(ERR_TYPE, node->token, "all types of return value is not integrated.");
      error(ERR_NOTE, types[0]->token, "was inferred as '%s' first", firststr.c_str());
      error(ERR_TYPE, (*it)->token, "expected '%s', but found '%s'", firststr.c_str(), eval.to_string().c_str());
      return;
    }
  }
}

std::pair<Node*, std::size_t> Evaluater::find_var(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
#if __DEBUG__
    alert;
    fprintf(stderr,"*it = %p\n",*it);
    fprintf(stderr,"(*it)->object.size() = %lu\n",(*it)->objects.size());
#endif

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
      alert;
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
      else if( node->name == "none" )
        ret = OBJ_NONE;
      else {
        error(ERR_TYPE, node->token, "unknown type name");
        exit(1);
      }

      break;
    }

    case NODE_ARGUMENT: {
      ret = evaluate(node->type);
      break;
    }

    case NODE_FUNCTION: {
      ret = evaluate(node->expr);
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
        ret = evaluate(i);
      }

      if( node != Global::get_instance()->top_node ) {
        must_integrated(node);
      }

      scope_list.pop_front();
      break;
    }

    case NODE_VAR: {
      if( !node->is_allowed_let ) {
        error(ERR_LOCATION, node->token, "cannot declare variable here");
        exit(1);
      }

      auto [scope, index] = find_var(node->name);

      if( scope ) {
        error(ERR_MULTIPLE_DEFINED, node->token, "multiple defined variable name");
        exit(1);
      }

      auto cur = *scope_list.begin();
      auto& obj = cur->objects.emplace_back();

      obj.name = node->name;

      node->var_scope = cur;
      node->var_index = cur->objects.size() - 1;

      if( node->type ) {
        obj.type = evaluate(node->type);
      }

      if( node->expr ) {
        auto expr_t = evaluate(node->expr);

        if( node->type && !expr_t.equals(obj.type) ) {
          error(ERR_TYPE, node->token, "type mismatch");
        }
        else {
          obj.type = expr_t;
        }
      }

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