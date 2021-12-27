#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

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

    case NODE_RETURN: {
      return { node->expr };
    }

    case NODE_SCOPE: {
      if( node->list.empty() ) {
        return { };
      }

      Vec v;
      Vec::const_iterator begin = node->list.end() - 1;

      for( auto it = node->list.begin(); it != node->list.end(); it++ ) {
        if( *it && (*it)->kind == NODE_RETURN ) {
          begin = it;
          break;
        }
      }

      for( auto it = begin; it >= node->list.begin(); it-- ) {
        for( auto&& i : get_return_values(*it) ) {
          v.emplace_back(i);
        }

        if( !is_branchable(*it) ) {
          break;
        }
        else { // keep this new line
          if( (*it)->kind == NODE_IF && (*it)->if_else && (*it)->if_else->kind != NODE_IF ) {
            break;
          }
        }
      }

      return v;
    }

    case NODE_FUNCTION: {
      return get_return_values(node->expr);
    }

    case NODE_VAR:
      return { };
  }

  return { node };
}
