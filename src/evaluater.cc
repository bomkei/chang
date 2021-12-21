#include "chang.h"

std::tuple<Node*, std::size_t> Evaluater::find_var(std::string_view const& name) {
  for( auto it = scope_list.rbegin(); it != scope_list.rend(); it++ ) {
    auto find = (*it)->find_var(name);

    if( find != -1 ) {
      return { *it, find };
    }
  }
  
  return { nullptr, 0 };
}

ObjectType Evaluater::evaluate(Node* node) {
  if( !node )
    return { };

  if( node->evaluated )
    return node->objtype;

  auto& ret = node->objtype;
  node->evaluated = true;

#if __DEBUG__
  assert(!scope_list.empty());
#endif

  switch( node->kind ) {
    case NODE_VALUE:
      ret = node->obj.type;
      break;
    
    case NODE_VARIABLE: {
      auto [scope, index] = find_var(node->name);

      if( scope ) {
        node->var_scope = scope;
        node->var_index = index;
        return scope->objects[index].type;
      }

      error(ERR_UNDEFINED, node->token, "undefined variable name '%s'", node->name.cbegin());
      exit(1);
    }

    case NODE_TYPE: {
      if( node->name == "int" )
        ret = OBJ_INT;
      else if( node->name == "none" )
        ret = OBJ_NONE;
      else {
        error(ERR_TYPE, node->token, "unknown type name");
        exit(1);
      }

      break;
    }

    case NODE_ARGUMENT: {
      ret = evaluate(node->type);
      break;
    }

    case NODE_FUNCTION: {
      ret = evaluate(node->expr);
      break;
    }

    case NODE_SCOPE: {
      if( node->list.empty() )
        break;
      
      ret = evaluate(*node->list.rbegin());
      break;
    }

    case NODE_VAR: {
      auto [scope, index] = find_var(node->name);

      
    }

    default: {
      auto lhs = evaluate(node->lhs);
      auto rhs = evaluate(node->rhs);

      if( !lhs.equals(rhs) ) {
        error(ERR_TYPE, node->token, "type mismatch");
        exit(1);
      }

      ret = lhs;
      break;
    }
  }
  
  return ret;
}