#include "chang.h"

std::string Object::to_string() const {
  switch( type.kind ) {
    case OBJ_INT:
      return std::to_string(v_int);

    case OBJ_NONE:
      return "none";
  }

  return "";
}