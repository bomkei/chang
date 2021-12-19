#include "chang.h"

Object Interpreter::run_node(Node* node) {
  if( !node )
    return { };
  
  switch( node->kind ) {
    case NODE_VALUE:
      return node->obj;

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