#include "chang.h"

std::tuple<Node*, std::size_t> Evaluater::find_var(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
    alert;
    fprintf(stderr,"*it = %p\n",*it);
    fprintf(stderr,"(*it)->object.size() = %lu\n",(*it)->objects.size());

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

  switch( node->kind ) {
    case NODE_VALUE:
      ret = node->obj.type;
      break;
    
    case NODE_VARIABLE: {
      auto [scope, index] = find_var(node->name);

      alert;
      fprintf("scope = %p\n");
      fprintf("index = %lu\n");

      if( scope ) {
        node->var_scope = scope;
        node->var_index = index;
        ret = scope->objects[index].type;
        break;
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
      alert;
      fprintf(stderr,"scope=%p\n",node);

      if( node->list.empty() ) {
        alert;
        break;
      }
      
      scope_list.push_front(node);
      
      ret = evaluate(*node->list.rbegin());

      scope_list.pop_front();
      break;
    }

    case NODE_VAR: {
      alert;

      auto [scope, index] = find_var(node->name);

      if( scope ) {
        error(ERR_MULTIPLE_DEFINED, node->token, "multiple defined variable name");
        exit(1);
      }

      auto cur = *scope_list.begin();
      auto& obj = cur->objects.emplace_back();

      obj.name = node->name;

      if( node->type ) {
        obj.type = evaluate(node->type);
      }

      if( node->expr ) {
        auto expr_t = evaluate(node->expr);

        if( node->type && !expr_t.equals(obj.type) ) {
          error(ERR_TYPE, node->token, "type mismatch");
        }
      }

      alert;
      std::cout << obj << std::endl;

      break;
    }

    case NODE_EXPR: {
      ret = evaluate(node->expr);

      for( auto&& item : node->expr_list ) {
        evaluate(item.item);
      }

      break;
    }
  }
  
  return ret;
}