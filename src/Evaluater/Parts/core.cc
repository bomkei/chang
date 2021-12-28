#include "error.h"
#include "debug.h"
#include "Utils.h"
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
    case NODE_ARRAY:
    case NODE_VARIABLE:
    case NODE_INDEX_REF:
    case NODE_REFERENCE:
    case NODE_CALLFUNC:
    case NODE_ARGUMENT:
      return primary(node);

    case NODE_TYPE:
      return type(node);

    case NODE_ASSIGN:
    case NODE_EXPR:
      return expr(node);

    case NODE_COMPARE:
      return compare(node);

    case NODE_IF:
    case NODE_VAR:
    case NODE_RETURN:
    case NODE_SCOPE:
      return stmt(node);

    case NODE_FUNCTION:
      return func(node);
  }

  return { };
}