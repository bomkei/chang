#include "chang.h"

bool ObjectType::equals(ObjectType const& type) const {
  if( kind == type.kind ) {
    // todo
    
    return true;
  }

  return false;
}

std::string Object::to_string() const {
  switch( type.kind ) {
    case OBJ_INT:
      return std::to_string(v_int);

    case OBJ_NONE:
      return "none";
  }

  return "";
}