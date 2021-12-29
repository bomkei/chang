#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Utils.h"
#include "Evaluater.h"
#include "Global.h"

ObjectType Evaluater::stmt(Node* node) {
  auto& ret = node->objtype;

  switch( node->kind ) {
    case NODE_SCOPE: {
      if( node->list.empty() ) {
        break;
      }

      scope_list.push_front(node);

      scope_depth++;
      node->scope_depth = scope_depth;

      auto returned = false;
      Node* ret_nd;

      for( auto&& i : node->list ) {
        if( !i ) {
          continue;
        }

        if( returned ) {
          if( i == Global::get_instance()->main_zero )
            continue;

          error(ERR_RETURN, i->token, "code is invalid after return statement");
          error(ERR_NOTE, ret_nd->token, "returned here");
        }
        else if( i->kind == NODE_RETURN ) {
          returned = true;
          ret_nd = i;
        }

        i->is_allowed_let = true;
        i->is_allowed_return = true;

        evaluate(i);
      }

      if( node != Global::get_instance()->top_node ) {
        auto const& ret_nodes = get_return_values(node);

        ret = must_integrated(node, ret_nodes);

        for( auto&& i : ret_nodes ) {
          auto&& e = evaluate(i);

          if( e.reference && node->scope_depth <= i->scope_depth ) {
            error(ERR_LIFE_SPAN, i->token, "this is cannot be take out and use from scope");
          }
        }
      }

      scope_depth--;
      scope_list.pop_front();

      break;
    }

    case NODE_VAR: {
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
      obj.scope_depth = cur->scope_depth;

      initialized[&obj] = false;

      // set pointer
      node->var_scope = cur;
      node->var_index = cur->objects.size() - 1;

      auto expr_t = evaluate(node->expr);

      if( node->expr ) {
        if( node->type ) {
          auto specify = evaluate(node->type);

          if( !specify.equals(expr_t) ) {
            error(ERR_TYPE, node->type->token, "expected '" + expr_t.to_string() + "'");
          }
        }

        obj.type = expr_t;
        node->is_reference = expr_t.reference;
      }
      else {
        obj.type = evaluate(node->type);

        if( obj.type.reference ) {
          error(ERR_VALUE_TYPE, node->token, "expected initializer expression");
          exit(1);
        }
      }

      initialized[&obj] = true;
      break;
    }

    case NODE_IF: {
      if( !evaluate(node->expr).equals(OBJ_BOOL) ) {
        error(ERR_TYPE, node->token, "condition is must boolean");
      }

      if( node->if_else ) {
        ret = evaluate(node->if_true);

        if( !ret.equals(evaluate(node->if_else)) ) {
          error(ERR_TYPE, node->token, "type mismatch");
        }

        return ret;
      }
      
      evaluate(node->if_true);
      break;
    }

    case NODE_RETURN: {
      if( !node->is_allowed_return ) {
        error(ERR_LOCATION, node->token, "cannot use return here");
      }

      return evaluate(node->expr);
    }
  }

  return ret;
}