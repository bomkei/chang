#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

Object Interpreter::run_node(Node* node) {
  if( !node )
    return { };
  
  switch( node->kind ) {
    case NODE_VALUE:
    case NODE_ARRAY:
    case NODE_VARIABLE:
    case NODE_INDEX_REF:
    case NODE_REFERENCE:
    case NODE_DEREFERENCE:
    case NODE_CALLFUNC:
    case NODE_ARGUMENT:
      return run_primary(node);

    case NODE_ASSIGN:
    case NODE_EXPR:
    case NODE_COMPARE:
      return run_expr(node);

    case NODE_IF:
    case NODE_VAR:
    case NODE_RETURN:
    case NODE_SCOPE:
      return run_stmt(node);

    case NODE_FUNCTION:
    case NODE_TYPE:
      break;
  }

  return { };
}