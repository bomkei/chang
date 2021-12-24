#include "chang.h"

bool ObjectType::equals(ObjectType const& type) const {
  if( kind == type.kind ) {
    if( kind == OBJ_TUPLE ) {
      // todo
    }

    if( arr_depth == type.arr_depth )
      return true;
  }

  return false;
}

std::string ObjectType::to_string() const {
  switch( kind ) {
    case OBJ_INT:
      return "int";
    
    case OBJ_CHAR:
      return "char";
    
    case OBJ_FLOAT:
      return "float";
    
    case OBJ_STRING:
      return "string";
    
    case OBJ_BOOL:
      return "bool";
    
    case OBJ_NONE:
      return "none";
  }

  return "";
}

std::string Object::to_string() const {
  switch( type.kind ) {
    case OBJ_INT:
      return std::to_string(v_int);

    case OBJ_CHAR:
      return Utils::String::to_utf8(std::u16string(1, v_str[0]));

    case OBJ_FLOAT:
      return Utils::remove_zero(std::to_string(v_float));

    case OBJ_BOOL:
      return v_bool ? "true" : "false";

    case OBJ_STRING:
      return Utils::String::to_utf8(v_str);

    case OBJ_NONE:
      return "none";
  }

  return "";
}