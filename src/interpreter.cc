#include "chang.h"

Object Interpreter::run_node(Node* node) {
  if( !node )
    return { };
  
  switch( node->kind ) {
    case NODE_VALUE:
      alert;
      return node->obj;

    case NODE_SCOPE: {
      Object obj;

      for( auto&& item : node->list ) {
        alert;
        obj = run_node(item);
      }

      return obj;
    }

    default: {
      auto lhs = run_node(node->lhs);
      auto rhs = run_node(node->rhs);

      switch( node->kind ) {
        case NODE_ADD: {
          lhs.v_int += rhs.v_int;
          break;
        }


      }

      return lhs;
    }
  }

  return { };
}