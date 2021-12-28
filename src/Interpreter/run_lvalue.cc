#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

Object& Interpreter::run_lvalue(Node* node) {
#if __DEBUG__
  assert(node != nullptr);
#endif

  switch( node->kind ) {
    case NODE_VARIABLE: {
      return node->get_var();
    }
  }

  error(ERR_TYPE, node->token,
    "omg this is not a lvalue, but why you can see this error?"
    PLEASE_REPORT "9oGb83NNmwx");
  exit(1);
}
