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
        alert;

        auto& x = *obj.address;

        alert;
        return x;
      }

      return obj;
    }

    case NODE_INDEX_REF: {
      auto&& arr = run_node(node->lhs);

      alert;

      auto& x = obj_index(arr, run_node(node->rhs).v_int, node->token);

      alert;

      return x;
    }

    case NODE_REFERENCE: {
      alert;
      return *(node->obj.address);
    }
  }

  alert;

  error(ERR_TYPE, node->token,
    "omg this is not a lvalue, but why this error have been occur?? "
    PLEASE_REPORT "9oGb83NNmwx");
  exit(1);
}
