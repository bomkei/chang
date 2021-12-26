#include "chang.h"

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

std::string Object::to_string() const {
  if( type.arr_depth ) {
    std::string str = "[";

    for( std::size_t i = 0; i < list.size(); i++ ) {
      str += list[i].to_string();
      if( i < list.size() - 1 ) str += ", ";
    }

    return str + "]";
  }

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
      if( v_str.empty() ) {
        return "\"\"";
      }

      return Utils::String::to_utf8(v_str);

    case OBJ_NONE:
      return "none";
  }

  return "";
}