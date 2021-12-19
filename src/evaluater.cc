#include "chang.h"

ObjectType Evaluater::evaluate(Node* node) {
  if( !node )
    return { };

  switch( node->kind ) {
    case NODE_VALUE:
      return node->obj.type;
    
    case NODE_FUNCTION: {

    }

    case NODE_SCOPE: {
      
    }

    default: {
      auto lhs = evaluate(node->lhs);
      auto rhs = evaluate(node->rhs);

      if( !lhs.equals(rhs) ) {
        error(ERR_TYPE, node->token, "type mismatch");
        exit(1);
      }

      return lhs;
    }
  }
  
  return { };
}