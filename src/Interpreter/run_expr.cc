#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

Object Interpreter::run_expr(Node* node) {
  switch( node->kind ) {
    case NODE_ASSIGN: {
      for( auto it = node->list.rbegin() + 1; it != node->list.rend(); it++ ) {
        run_lvalue(*it) = run_node(*(it - 1));
      }

      return run_lvalue(node->expr) = run_node(*node->list.begin());
    }

    case NODE_EXPR: {
      auto obj = run_node(node->expr);

      for( auto&& pair : node->expr_list ) {
        auto&& item = run_node(pair.item);

        switch( pair.kind ) {
          case EXPR_ADD:
            Interpreter::add(obj, item);
            break;

          case EXPR_SUB:
            Interpreter::sub(obj, item);
            break;

          case EXPR_MUL:
            Interpreter::mul(obj, item);
            break;

          case EXPR_DIV:
            Interpreter::div(obj, item);
            break;
        }
      }

      return obj;
    }
  }

  return { };
}