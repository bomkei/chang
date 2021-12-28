#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "BuiltinFunc.h"
#include "Interpreter.h"

Object Interpreter::run_primary(Node* node) {
  switch( node->kind ) {
    case NODE_VALUE:
    case NODE_REFERENCE: {
      return node->obj;
    }

    case NODE_VARIABLE:
      return run_lvalue(node);

    case NODE_INDEX_REF: {
      auto obj = run_node(node->lhs);
      auto index = run_node(node->rhs).v_int;

      if( index < 0 || index >= obj.list.size() ) {
        error(ERR_OUT_OF_RANGE, node->token, "subscript out of range");
      }

      return obj.list[index];
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