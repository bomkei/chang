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
      auto obj = run_node(node->expr_list[0].item);

      if( node->is_single() ) {
        return obj;
      }

      for( auto it = node->expr_list.begin() + 1; it != node->expr_list.end(); it++ ) {
        auto& pair = *it;
        auto item = run_node(pair.item);

        Interpreter::expr_obj(pair.kind, obj, item);

        obj = std::move(item);
      }

      return obj;
    }

    case NODE_COMPARE: {
      auto obj = run_node(node->expr_list[0].item);

      for( auto it = node->expr_list.begin() + 1; it != node->expr_list.end(); it++ ) {
        auto&& x = run_node(it->item);

        if( !compare_obj(obj, x) )
          goto cmp_failure;
        
        obj = std::move(x);
      }

      obj.type = OBJ_BOOL;
      obj.v_bool = true;

      return obj;

    cmp_failure:
      obj.type = OBJ_BOOL;
      obj.v_bool = 0;

      return obj;
    }
  }

  return { };
}