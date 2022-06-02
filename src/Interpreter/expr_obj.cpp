#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

void Interpreter::expr_obj(ExprKind kind, Object& a, Object& b) {
  auto const kk = a.type.kind;

  if( a.type.arr_depth ) {
    for( auto&& i : b.list ) {
      a.list.emplace_back(i);
    }

    return;
  }

  switch( kind ) {
    case EXPR_ADD: {
      switch( kk ) {
        case OBJ_INT: a.v_int += b.v_int; break;
        case OBJ_FLOAT: a.v_float += b.v_float; break;
        case OBJ_STRING: a.v_str += b.v_str; break;
      }
      break;
    }

    case EXPR_SUB: {
      switch( kk ) {
        case OBJ_INT: a.v_int -= b.v_int; break;
        case OBJ_FLOAT: a.v_float -= b.v_float; break;
      }
      break;
    }

    case EXPR_MUL: {
      switch( kk ) {
        case OBJ_INT: a.v_int *= b.v_int; break;
        case OBJ_FLOAT: a.v_float *= b.v_float; break;
      }
      break;
    }

    case EXPR_DIV: {
      switch( kk ) {
        case OBJ_INT: a.v_int /= b.v_int; break;
        case OBJ_FLOAT: a.v_float /= b.v_float; break;
      }
      break;
    }
    
    case EXPR_LSHIFT: {
      a.v_int <<= b.v_int;
      break;
    }

    case EXPR_RSHIFT: {
      a.v_int >>= b.v_int;
      break;
    }

    case EXPR_SPACESHIP: {
      switch( kk ) {
        case OBJ_INT: a.v_int = (a.v_int < b.v_int ? -1 : a.v_int > b.v_int);
        case OBJ_FLOAT: a.v_float = (a.v_float < b.v_float ? -1 : a.v_float > b.v_float);
      }
      break;
    }
  }
}

bool Interpreter::compare_obj(ExprKind kind, Object& a, Object& b) {
  switch( a.type.kind ) {
    case OBJ_INT:
      switch( kind ) {
        case EXPR_LBIGGER: return a.v_int > b.v_int;
        case EXPR_RBIGGER: return a.v_int < b.v_int;
        case EXPR_LBIGGER_OR_EQ: return a.v_int >= b.v_int;
        case EXPR_RBIGGER_OR_EQ: return a.v_int <= b.v_int;
        case EXPR_EQUAL: return a.v_int == b.v_int;
        case EXPR_NOT_EQUAL: return a.v_int != b.v_int;
      }
      break;
      
    case OBJ_FLOAT:
      switch( kind ) {
        case EXPR_LBIGGER: return a.v_float > b.v_float;
        case EXPR_RBIGGER: return a.v_float < b.v_float;
        case EXPR_LBIGGER_OR_EQ: return a.v_float >= b.v_float;
        case EXPR_RBIGGER_OR_EQ: return a.v_float <= b.v_float;
        case EXPR_EQUAL: return a.v_float == b.v_float;
        case EXPR_NOT_EQUAL: return a.v_float != b.v_float;
      }
      break;
  }

  return false;
}
