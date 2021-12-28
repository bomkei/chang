#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

ObjectType Evaluater::expr(Node* node) {
  auto& ret = node->objtype;

  switch( node->kind ) {
    case NODE_ASSIGN: {
      if( !is_lvalue(node->expr) ) {
        error(ERR_VALUE_TYPE, node->expr->token, "expression is must lvalue");
      }

      ret = evaluate(node->expr);

      for( auto&& i : node->list ) {
        if( i != *node->list.rbegin() && !is_lvalue(i) ) {
          error(ERR_VALUE_TYPE, i->token, "expression is must lvalue");
        }

        if( !ret.equals(evaluate(i)) ) {
          error(ERR_TYPE, i->token, "type mismatch");
        }
      }

      break;
    }

    case NODE_EXPR: {
      ret = evaluate(node->expr_list[0].item);

      if( node->is_single() ) {
        return ret;
      }

      for( auto it = node->expr_list.begin() + 1; it != node->expr_list.end(); it++ ) {
        auto& item = *it;
        auto type = evaluate(item.item);

        if( !type.equals(ret) ) {
          error(ERR_TYPE, item.token, "type mismatch");
        }

        if( ret.arr_depth && item.kind != EXPR_ADD ) {
          error(ERR_OPERATOR, item.token, "invalid operator for array");
        }

        switch( type.kind ) {
          case OBJ_STRING: {
            switch( item.kind ) {
              case EXPR_ADD:
                break;
              
              default: {
                error(ERR_OPERATOR, item.token, "invalid operator for '%s'", type.to_string().c_str());
                exit(1);
              }
            }
          }
        }
      }

      break;
    }
  }

  return ret;
}