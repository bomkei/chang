#include "chang.h"

bool Evaluater::is_branchable(Node* node) {
  if( !node )
    return false;

  switch( node->kind ) {
    case NODE_IF:
      return true;
  }

  return false;
}

std::vector<Node*> Evaluater::get_return_values(Node* node) {
  using Vec = std::vector<Node*>;

  if( !node )
    return { };

  switch( node->kind ) {
    case NODE_IF: {
      Vec v;

      for( auto&& i : get_return_values(node->if_true) ) {
        v.emplace_back(i);
      }

      for( auto&& i : get_return_values(node->if_else) ) {
        v.emplace_back(i);
      }

      return v;
    }

    case NODE_SCOPE: {
      Vec v;

      for( auto it = node->list.rbegin(); it != node->list.rend(); it++ ) {
        for( auto&& i : get_return_values(*it) ) {
          v.emplace_back(i);
        }

        if( !is_branchable(*it) ) {
          break;
        }
        else {
          if( (*it)->kind == NODE_IF && (*it)->if_else ) {
            break;
          }
        }
      }

      return v;
    }

    case NODE_FUNCTION: {
      return get_return_values(node->expr);
    }
  }

  return { node };
}

std::pair<bool, Node*> Evaluater::is_integrated(Node* node) {
  ObjectType type;
  auto&& vec = get_return_values(node);

  for( auto it = vec.begin(); it != vec.end(); it++ ) {
    if( it == vec.begin() ) {
      type = evaluate(*it);
      continue;
    }
    
    if( !type.equals(evaluate(*it)) ) {
      return { false, *it };
    }
  }

  return { true, nullptr };
}

std::pair<Node*, std::size_t> Evaluater::find_var(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
#if __DEBUG__
    alert;
    fprintf(stderr,"*it = %p\n",*it);
    fprintf(stderr,"(*it)->object.size() = %lu\n",(*it)->objects.size());
#endif

    auto find = (*it)->find_var(name);

    if( find != -1 ) {
      return { *it, find };
    }
  }
  
  return { nullptr, 0 };
}

Node* Evaluater::find_func(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
    for( auto&& i : (*it)->list ) {
      if( i->kind == NODE_FUNCTION && i->name == name ) {
        return i;
      }
    }
  }

  return nullptr;
}