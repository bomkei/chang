#include "Object.h"
#include "Utils.h"

std::string Object::to_string() const {
  if( type.reference ) {
    return address->to_string();
  }

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