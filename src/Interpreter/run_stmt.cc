#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
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

      if( node->is_make_array ) {
        auto ec_it = node->objects.begin();

        for( auto&& ec : node->type->elemcount_list ) {
          if( ec != nullptr ) {
            *ec_it++ = run_node(ec);
          }
        }
      }

      if( !node->expr ) {
        if( node->is_make_array )
          node->get_var() = construct_array(node->type->objtype.kind, node->objects.rend(), node->objects.rbegin());
      }
      else {
        node->get_var() = run_node(node->expr);
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