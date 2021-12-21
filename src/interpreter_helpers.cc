#include "chang.h"

void Interpreter::add(Object& obj, Object& item) {
  switch( obj.type.kind ) {
    case OBJ_INT:
      obj.v_int += item.v_int;
      break;
  }
}

void Interpreter::sub(Object& obj, Object& item) {
  switch( obj.type.kind ) {
    case OBJ_INT:
      obj.v_int -= item.v_int;
      break;
  }
}

void Interpreter::mul(Object& obj, Object& item) {
  switch( obj.type.kind ) {
    case OBJ_INT:
      obj.v_int *= item.v_int;
      break;
  }
}

void Interpreter::div(Object& obj, Object& item) {
  switch( obj.type.kind ) {
    case OBJ_INT:
      obj.v_int /= item.v_int;
      break;
  }
}

