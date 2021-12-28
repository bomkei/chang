#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

void Interpreter::expr_obj(ExprKind kind, Object& a, Object& b) {
  if( a.type.arr_depth ) {
    for( auto&& i : b.list ) {
      a.list.emplace_back(i);
    }

    return;
  }

  switch( kind ) {
    case EXPR_ADD: {
      switch( a.type.kind ) {
        case OBJ_INT:
          a.v_int += b.v_int;
          break;
      }
    }
  }
}
