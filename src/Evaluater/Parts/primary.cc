#include "error.h"
#include "debug.h"
#include "Utils.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Evaluater.h"

ObjectType Evaluater::primary(Node* node) {
  auto& ret = node->objtype;

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

        ret = node->get_var().type;
        break;
      }

      error(ERR_UNDEFINED, node->token, "undefined variable name '" + Utils::str(node->name) + "'");
      exit(1);
    }

    case NODE_INDEX_REF: {
      ret = evaluate(node->lhs);

      if( ret.arr_depth == 0 ) {
        error(ERR_TYPE, node->token, "left side is must array");
      }

      if( !evaluate(node->rhs).equals(OBJ_INT) ) {
        error(ERR_TYPE, node->rhs->token, "index is must integer");
      }

      ret.arr_depth--;
      break;
    }

    case NODE_REFERENCE: {
      if( !is_lvalue(node->expr) ) {
        error(ERR_VALUE_TYPE, node->expr->token, "expression is must lvalue");
        exit(1);
      }

      ret = evaluate(node->expr);

      if( ret.reference ) {
        error(ERR_TYPE, node->token, "cannot nest reference");
        exit(1);
      }

      ret.reference = true;

      node->scope_depth = node->expr->scope_depth;
      node->obj.type = ret;

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
      std::vector<ObjectType> call_args;
      std::vector<ObjectType> func_args;
      Node* find;

      for( auto&& i : node->list ) {
        call_args.emplace_back(evaluate(i));
      }

      // find builtin
      for( auto&& i : builtin ) {
        if( i.name == node->name ) {
          argc = i.arg_types.size();
          arg_free = i.arg_free;
          node->builtin = &i;
          ret = i.ret_type;
          goto check_process;
        }
      }

      find = find_func(node->name);

      if( !find ) {
        error(ERR_UNDEFINED, node->token, "undefined function name");
        exit(1);
      }

      func_args = eval_func_args(find);

      if( call_args.size() != func_args.size() ) {
        error(ERR_ARGUMENT, node->token, "no match arguments count");
      }
      else {
        for( std::size_t i = 0; i < call_args.size(); i++ ) {
          auto const& c = call_args[i];
          auto const& f = func_args[i];

          if( !c.equals(f) ) {
            error(ERR_TYPE, node->list[i]->token, "expected '" + f.to_string() + "', but found '" + c.to_string() + "'");
          }
        }
      }
      
      ret = evaluate(find->expr);
      node->func = find;
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

    case NODE_ARGUMENT:
      ret = evaluate(node->type);
      break;
  }

  return ret;
}