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
      auto& obj = node->get_var();

      if( obj.type.reference ) {
        return *obj.address;
      }

      return obj;
    }

    case NODE_INDEX_REF: {
      auto&& arr = run_node(node->lhs);
      return obj_index(arr, run_node(node->rhs).v_int, node->token);
    }

    case NODE_REFERENCE: {
      return *(node->obj.address);
    }
  }

  error(ERR_TYPE, node->token,
    "omg this is not a lvalue, but why can you see this error? "
    PLEASE_REPORT "9oGb83NNmwx");
  exit(1);
}
