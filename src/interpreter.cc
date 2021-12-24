#include "chang.h"

static Interpreter* _instance;

Interpreter::Interpreter() {
  _instance = this;
}

Interpreter* Interpreter::get_instance() {
  return _instance;
}

void Interpreter::add(Object& obj, Object& item) {
  switch( obj.type.kind ) {
    case OBJ_INT:
      obj.v_int += item.v_int;
      break;
  }
}

void Interpreter::sub(Object& obj, Object& item) {
  switch( obj.type.kind ) {
    case OBJ_INT:
      obj.v_int -= item.v_int;
      break;
  }
}

void Interpreter::mul(Object& obj, Object& item) {
  switch( obj.type.kind ) {
    case OBJ_INT:
      obj.v_int *= item.v_int;
      break;
  }
}

void Interpreter::div(Object& obj, Object& item) {
  switch( obj.type.kind ) {
    case OBJ_INT:
      obj.v_int /= item.v_int;
      break;
  }
}

Object& Interpreter::run_lvalue(Node* node) {
#if __DEBUG__
  assert(node != nullptr);
#endif

  switch( node->kind ) {
    case NODE_VARIABLE: {
      return node->get_var();
    }
  }

  alert;
  abort();
}

Object Interpreter::run_node(Node* node) {
  if( !node )
    return { };
  
  switch( node->kind ) {
    case NODE_VALUE:
      alert;
      return node->obj;

    case NODE_VARIABLE:
      return run_lvalue(node);

    case NODE_ARRAY: {
      Object obj;

      obj.type = node->objtype;

      if( node->is_allowed_empty_array ) {
        
      }
      else {

      }

      break;
    }

    case NODE_CALLFUNC: {
      std::vector<Object> args;

      for( auto&& i : node->list ) {
        args.emplace_back(run_node(i));
      }

      if( node->builtin ) {
        return node->builtin->func(args);
      }

      break;
    }

    case NODE_SCOPE: {
      Object obj;

      for( auto&& item : node->list ) {
        alert;
        obj = run_node(item);
      }

      return obj;
    }
  
    case NODE_VAR: {
      // if( node->is_make_array ) {
      //   for( std::size_t i = 0; i < node->elemcount_list.size(); i++ ) {
      //     node->objects[i] = run_node(node->elemcount_list[i]);
      //   }
      // }

      auto ec_it = node->objects.begin();

      for( auto&& ec : node->elemcount_list ) {
        if( ec != nullptr ) {
          *ec_it++ = run_node(ec);
          //ec_it++;
        }
      }

      node->get_var() = run_node(node->expr);
      
      break;
    }

    case NODE_EXPR: {
      auto obj = run_node(node->expr);

      for( auto&& pair : node->expr_list ) {
        auto&& item = run_node(pair.item);

        switch( pair.kind ) {
          case EXPR_ADD:
            Interpreter::add(obj, item);
            break;

          case EXPR_SUB:
            Interpreter::sub(obj, item);
            break;

          case EXPR_MUL:
            Interpreter::mul(obj, item);
            break;

          case EXPR_DIV:
            Interpreter::div(obj, item);
            break;
        }
      }

      return obj;
    }
  }

  return { };
}