#include "chang.h"

ObjectType Evaluater::evaluate(Node* node) {
  if( !node )
    return { };

#if __DEBUG__
  fprintf(stderr,"node->kind = %d\n",node->kind);
#endif

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

#if __DEBUG__
      alert;
      fprintf(stderr,"scope = %p\n",scope);
      fprintf(stderr,"index = %lu\n",index);
#endif

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
      alert;
      for( auto&& i : get_return_values(node) ) {
        error(ERR_NOTE, i->token, "");
      }

      if( node->list.empty() ) {
        alert;
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
        evaluate(item.item);
      }

      break;
    }
  }
  
  return ret;
}