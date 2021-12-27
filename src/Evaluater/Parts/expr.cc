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