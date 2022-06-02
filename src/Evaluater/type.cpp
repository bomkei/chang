#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

ObjectType Evaluater::type(Node* node) {
  auto& ret = node->objtype;

  if( node->name == "int" )
    ret = OBJ_INT;
  else if( node->name == "char" )
    ret = OBJ_CHAR;
  else if( node->name == "float" )
    ret = OBJ_FLOAT;
  else if( node->name == "bool" )
    ret = OBJ_BOOL;
  else if( node->name == "string" )
    ret = OBJ_STRING;
  else if( node->name == "none" )
    ret = OBJ_NONE;
  else {
    // todo: find struct

    error(ERR_TYPE, node->token, "unknown type name");
    exit(1);
  }

  for( auto&& i : node->elemcount_list ) {
    if( i && !evaluate(i).equals(OBJ_INT) ) {
      error(ERR_TYPE, i->token, "elements count is must be integer");
    }

    ret.arr_depth++;
  }

  ret.reference = node->is_reference;
  return ret;
}