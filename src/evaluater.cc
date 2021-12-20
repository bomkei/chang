#include "chang.h"

ObjectType Evaluater::evaluate(Node* node) {
  if( !node )
    return { };

  if( node->evaluated )
    return node->objtype;

  auto& ret = node->objtype;
  node->evaluated = true;

  switch( node->kind ) {
    case NODE_VALUE:
      ret = node->obj.type;
      break;
    
    case NODE_TYPE: {
      if( node->name == "int" )
        ret = OBJ_INT;
      else if( node->name == "none" )
        ret = OBJ_NONE;
      else {
        error(ERR_TYPE, node->token, "unknown type name");
        exit(1);
      }

      break;
    }

    case NODE_ARGUMENT: {
      ret = evaluate(node->type);
      break;
    }

    case NODE_FUNCTION: {
      ret = evaluate(node->expr);
      break;
    }

    case NODE_SCOPE: {
      if( node->list.empty() )
        break;
      
      ret = evaluate(*node->list.rbegin());
      break;
    }

    default: {
      auto lhs = evaluate(node->lhs);
      auto rhs = evaluate(node->rhs);

      if( !lhs.equals(rhs) ) {
        error(ERR_TYPE, node->token, "type mismatch");
        exit(1);
      }

      ret = lhs;
      break;
    }
  }
  
  return ret;
}