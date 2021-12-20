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

    case NODE_VAR: {

      break;
    }

    case NODE_EXPR: {
      auto obj = run_node(node->expr);

      for( auto&& pair : node->expr_list ) {
        auto&& item = run_node(pair.item);

        switch( pair.kind ) {
          case EXPR_ADD:
            
        }
      }

      return obj;
    }
  }

  return { };
}