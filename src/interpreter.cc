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

Object Interpreter::construct_array(ObjectType type, Interpreter::EcObjIt end, Interpreter::EcObjIt it) {
  Object obj;
  obj.type = type;
  obj.type.arr_depth = 1;

  Object elem;
  elem.type = type;
  elem.type.arr_depth = 0;

  while( true ) {
    for( long i = 0; i < it->v_int; i++ ) {
      obj.list.emplace_back(elem);
    }

    if( it->v_int == 0 && elem.type.arr_depth ) {
      obj.list.emplace_back(elem);
    }

    if( ++it == end )
      break;

    elem = obj;
    obj.type.arr_depth++;
    obj.list.clear();
  }

  return obj;
}

void Interpreter::fit_array_length(std::vector<Object>::const_iterator const& ec_obj_it, Object& arr) {
  


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
  error(ERR_TYPE, node->token,
    "omg this is not a lvalue, but why you can see this error?"
    PLEASE_REPORT "$9oGb83NNmwx");
}

Object Interpreter::run_node(Node* node) {
  if( !node )
    return { };
  
  switch( node->kind ) {
    case NODE_VALUE:
      return node->obj;

    case NODE_VARIABLE:
      return run_lvalue(node);

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
        return construct_array(node->objtype, var_s->objects.rend(), var_s->objects.rbegin());
      }
      else {
        for( auto&& i : node->list ) {
          obj.list.emplace_back(run_node(i));
        }
      }

      return obj;
    }

    case NODE_SCOPE: {
      Object obj;

      for( auto&& item : node->list ) {
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
        node->get_var() = construct_array(node->type->objtype, node->objects.rend(), node->objects.rbegin());
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