#include <algorithm>
#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

static const std::vector<std::pair<std::vector<ExprKind>, std::vector<ObjectKind>>> valid_expr_list = {
  { { EXPR_ADD }, {
      OBJ_INT,
      OBJ_FLOAT,
      OBJ_STRING
  }},
  { { EXPR_SUB, EXPR_MUL, EXPR_DIV }, {
      OBJ_INT,
      OBJ_FLOAT
  }},
  { { EXPR_LSHIFT, EXPR_RSHIFT }, {
      OBJ_INT
  }},
  { { EXPR_SPACESHIP }, {
      OBJ_INT,
      OBJ_FLOAT
  }},
  { { EXPR_LBIGGER, EXPR_RBIGGER, EXPR_LBIGGER_OR_EQ, EXPR_RBIGGER_OR_EQ }, {
      OBJ_INT,
      OBJ_FLOAT
  }},
  { { EXPR_BIT_AND, EXPR_BIT_OR, EXPR_BIT_XOR }, {
      OBJ_INT
  }},
  { { EXPR_AND, EXPR_OR }, {
      OBJ_BOOL
  }}
};

bool check_expr(ExprKind kind, ObjectType type) {
  switch( kind ) {
    case EXPR_ADD: {
      if( type.arr_depth )
        return true;

      break;
    }

    case EXPR_EQUAL:
    case EXPR_NOT_EQUAL:
      return true;
  }

  for( auto&& chk : valid_expr_list ) {
    for( auto&& k : std::get<0>(chk) ) {
      if( k == kind ) {
        for( auto&& i : std::get<1>(chk) ) {
          if( type.equals(i) )
            return true;
        }

        return false;
      }
    }
  }

  return false;
}

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
        else if( !check_expr(item.kind, ret) ) {
          error(ERR_OPERATOR, item.token, "invalid operator");
          exit(1);
        }
      }

      break;
    }
  }

  return ret;
}