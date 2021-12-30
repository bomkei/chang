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

    case NODE_INDEX_REF: {
      auto& arr = run_lvalue(node->lhs);

      auto& x = obj_index(arr, run_node(node->rhs).v_int, node->token);

      return x;
    }

    case NODE_REFERENCE: {
      return *(node->obj.address);
    }

    case NODE_DEREFERENCE: {
      return *run_lvalue(node->expr).address;
    }
  }

  error(ERR_TYPE, node->token,
    "omg this is not a lvalue, but why this error have been occur?? "
    PLEASE_REPORT "9oGb83NNmwx");
  exit(1);
}
