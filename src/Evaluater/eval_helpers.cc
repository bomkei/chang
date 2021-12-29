#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

bool Evaluater::is_branchable(Node* node) {
  if( !node )
    return false;

  switch( node->kind ) {
    case NODE_IF:
      return true;
  }

  return false;
}

bool Evaluater::is_lvalue(Node* node) {
  assert(node != nullptr);

  switch( node->kind ) {
    case NODE_VARIABLE:
      return true;

    case NODE_INDEX_REF: {
      if( is_lvalue(node->expr) ) {
        node->scope_depth = node->expr->scope_depth;
        return true;
      }

      break;
    }

    case NODE_REFERENCE:
      if( is_lvalue(node->expr) ) {
        node->scope_depth = node->expr->scope_depth;
        return true;
      }

      break;
  }

  return false;
}

std::pair<Node*, std::size_t> Evaluater::find_var(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
    auto find = (*it)->find_var(name);

    if( find != -1 ) {
      return { *it, find };
    }
  }
  
  return { nullptr, 0 };
}

std::vector<ObjectType> Evaluater::eval_func_args(Node* func) {
  std::vector<ObjectType> ret;

  for( auto&& i : func->list ) {
    ret.emplace_back(evaluate(i->type));
  }

  return ret;
}

std::vector<Node*> Evaluater::find_func(std::string_view const& name, std::vector<ObjectType> const& arg_types) {
  std::vector<Node*> ret;

  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
    for( auto&& i : (*it)->list ) {
      if( i && i->kind == NODE_FUNCTION && i->name == name ) {
        auto args = eval_func_args(i);

        if( args.size() != arg_types.size() ) {
          continue;
        }
        else {
          for( std::size_t i = 0; i < args.size(); i++ ) {
            if( !args[i].equals(arg_types[i]) ) {
              goto cnt_label;
            }
          }
        }

        ret.emplace_back(i);
      cnt_label:;
      }
    }
  }

  return ret;
}
