#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Utils.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

Object& Interpreter::obj_index(Object& obj, long index, Token* token) {
  if( index < 0 || index >= obj.list.size() ) {
    alert;
    error(ERR_OUT_OF_RANGE, token, "index out of range");
    exit(1);
  }

  return obj.list[index];
}
