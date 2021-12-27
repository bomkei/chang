#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

ObjectType Evaluater::evaluate(Node* node) {
  if( !node )
    return { };

  if( node->evaluated )
    return node->objtype;

  node->evaluated = true;

  switch( node->kind ) {
    case NODE_VALUE:
    case NODE_VARIABLE:
    case NODE_CALLFUNC:
      return primary(node);
  }

  return { };
}