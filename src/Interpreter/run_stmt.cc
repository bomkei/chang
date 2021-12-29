#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Utils.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

Object Interpreter::run_stmt(Node* node) {
  switch( node->kind ) {
    case NODE_SCOPE: {
      Object obj;

      for( auto&& item : node->list ) {
        if( item && item->kind == NODE_RETURN ) {
          return run_node(item->expr);
        }

        obj = run_node(item);
      }

      return obj;
    }
  
    case NODE_VAR: {
      var_stmt_list.push_front(node);

      auto& obj = node->get_var();

      if( node->expr ) {
        obj = run_node(node->expr);
      }
      else {
        obj.type = node->type->objtype;
      }

      var_stmt_list.pop_front();
      break;
    }

    case NODE_IF: {
      if( run_node(node->expr).v_bool ) {
        return run_node(node->if_true);
      }
      
      return run_node(node->if_else);
    }
  }

  return { };
}