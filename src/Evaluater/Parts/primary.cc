#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

ObjectType Evaluater::primary(Node* node) {
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

        node->scope_depth = node->get_var().scope_depth;

        if( initialized.contains(&node->get_var()) && !initialized[&node->get_var()] ) {
          error(ERR_UNINITIALIZED, node->token, "cannot use uninitialized variable");
          exit(1);
        }

        alert;
      #if __DEBUG__
        fprintf(stderr,"node->scope_depth = %lu\n",node->scope_depth);
      #endif

        //ret = scope->objects[index].type;
        ret = node->get_var().type;

        break;
      }

      error(ERR_UNDEFINED, node->token, "undefined variable name '%s'", Utils::str(node->name));
      exit(1);
    }

    case NODE_REFERENCE: {
      if( !is_lvalue(node->expr) ) {
        error(ERR_VALUE_TYPE, node->expr->token, "expression is must lvalue");
        exit(1);
      }

      ret = evaluate(node->expr);
      ret.reference = true;

      // node->expr == NODE_VARIABLE
      
      node->scope_depth = node->expr->scope_depth;
      node->obj.type = ret;
      node->obj.address = node->objptr = get_obj_addr(node->expr);

      alert;
    #if __DEBUG__
      fprintf(stderr,"node->scope_depth = %lu\n",node->scope_depth);
    #endif

      break;
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

      // todo: check finely
      // todo: add find_func_same_name()

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
  }

  return { };
}