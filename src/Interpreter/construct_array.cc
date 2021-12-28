#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

Object Interpreter::construct_array(ObjectKind kind, Interpreter::EcObjIt end, Interpreter::EcObjIt it) {
  Object obj, elem;

  (obj.type = elem.type = kind).arr_depth = 0;
  obj.type.arr_depth = 1;

  while( true ) {
    for( long i = 0; i < it->v_int; i++ ) {
      obj.list.emplace_back(elem);
    }

    if( ++it == end )
      break;

    elem = obj;
    obj.type.arr_depth++;
    obj.list.clear();
  }

  return obj;
}

