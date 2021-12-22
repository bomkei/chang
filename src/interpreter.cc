#include "chang.h"

Object& Interpreter::run_lvalue(Node* node) {
#if __DEBUG__
  assert(node != nullptr);
#endif

  switch( node->kind ) {
    case NODE_VARIABLE: {
      return node->var_scope->objects[node->var_index];
    }
  }

  alert;
  abort();
}

Object Interpreter::run_node(Node* node) {
  if( !node )
    return { };
  
  switch( node->kind ) {
    case NODE_VALUE:
      alert;
      return node->obj;

    case NODE_VARIABLE:
      return run_lvalue(node);

    case NODE_CALLFUNC: {

      break;
    }

    case NODE_SCOPE: {
      Object obj;

      for( auto&& item : node->list ) {
        alert;
        obj = run_node(item);
      }

      return obj;
    }
  
    case NODE_VAR: {
      node->var_scope->objects[node->var_index] = run_node(node->expr);
      break;
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