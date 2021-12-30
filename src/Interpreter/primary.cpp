#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Utils.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

Object Interpreter::run_primary(Node* node) {
  switch( node->kind ) {
    case NODE_VALUE: {
      return node->obj;
    }

    case NODE_VARIABLE: {
      alert;
      auto& x = run_lvalue(node);

      alert;
      return x;
    }

    case NODE_REFERENCE: {
      auto&& x = run_lvalue(node->expr);
      
      node->obj.address = &x;

      debug(
        alert;
        printval(p,node->obj.address);
      )

      return node->obj;
    }

    case NODE_DEREFERENCE: {
      alert;
      auto&& x = run_lvalue(node->expr);

      assert(x.address);


      return *x.address;
    }

    case NODE_INDEX_REF: {
      auto obj = run_node(node->lhs);
      auto index = run_node(node->rhs).v_int;

      return obj_index(obj, index, node->token);
    }

    case NODE_CALLFUNC: {
      std::vector<Object> args;
      std::vector<Object> save;

      for( auto&& i : node->list ) {
        args.emplace_back(run_node(i));
      }

      if( node->builtin ) {
        return node->builtin->func(args);
      }

      const auto argc = node->func->list.size();
      for( std::size_t i = 0; i < argc; i++ ) {
        save.emplace_back(node->func->expr->objects[i]);
        node->func->expr->objects[i] = args[i];
      }

      auto obj = run_node(node->func->expr);

      for( std::size_t i = 0; i < argc; i++ ) {
        node->func->expr->objects[i] = save[i];
      }

      return obj;
    }

    case NODE_ARRAY: {
      Object obj;

      obj.type = node->objtype;

      if( node->is_allowed_empty_array ) {
        auto var_s = *var_stmt_list.begin();
        return construct_array(node->objtype.kind, var_s->objects.rend(), var_s->objects.rbegin());
      }
      else {
        for( auto&& i : node->list ) {
          obj.list.emplace_back(run_node(i));
        }
      }

      return obj;
    }
  }

  return { };
}