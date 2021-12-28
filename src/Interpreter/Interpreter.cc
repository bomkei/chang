#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

static Interpreter* _instance;

Interpreter::Interpreter() {
  _instance = this;
}

Interpreter* Interpreter::get_instance() {
  return _instance;
}
