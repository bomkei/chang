#include "Object.h"

bool ObjectType::equals(ObjectType const& type) const {
  if( kind == type.kind ) {
    if( kind == OBJ_TUPLE ) {
      // todo
    }

    if( arr_depth == type.arr_depth && reference == type.reference )
      return true;
  }

  return false;
}

std::string ObjectType::to_string() const {
  std::string name;

  switch( kind ) {
    case OBJ_INT:
      name = "int";
      break;
    
    case OBJ_CHAR:
      name = "char";
      break;
    
    case OBJ_FLOAT:
      name = "float";
      break;
    
    case OBJ_STRING:
      name = "string";
      break;
    
    case OBJ_BOOL:
      name = "bool";
      break;
    
    case OBJ_NONE:
      name = "none";
      break;
  }

  for( std::size_t i = 0; i < arr_depth; i++ ) {
    name += "[]";
  }

  if( reference )
    name += "&";

  return name;
}
