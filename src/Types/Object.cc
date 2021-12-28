#include "Object.h"
#include "Utils.h"

Object::Object(Object&& obj)
  : type(std::move(obj.type)),
    name(obj.name),
    scope_depth(obj.scope_depth),
    v_int(obj.v_int),
    v_str(std::move(obj.v_str)),
    list(std::move(obj.list))
{
}

Object& Object::operator = (Object&& obj) {
  type = std::move(obj.type);
  name = obj.name;
  scope_depth = obj.scope_depth;
  v_int = obj.v_int;
  v_str = std::move(obj.v_str);
  list = std::move(obj.list);
  
  return *this;
}

bool Object::equals(Object const& obj) const {
  if( !type.equals(obj.type) ) {
    return false;
  }

  if( type.reference ) {
    return address->equals(obj);
  }

  if( type.kind == OBJ_TUPLE || type.arr_depth ) {
    if( list.size() != obj.list.size() )
      return false;
    
    for( std::size_t i = 0; i < list.size(); i++ ) {
      if( !list[i].equals(obj.list[i]) ) {
        return false;
      }
    }
  }

  switch( type.kind ) {
    case OBJ_INT: return v_int == obj.v_int;
    case OBJ_CHAR: return v_char == obj.v_char;
    case OBJ_FLOAT: return v_float == obj.v_float;
    case OBJ_STRING: return v_str == obj.v_str;
    case OBJ_BOOL: return v_bool == obj.v_bool;
    case OBJ_NONE: return true;
  }

  return true;
}

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