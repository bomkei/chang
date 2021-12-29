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
      if( is_lvalue(node->lhs) ) {
        node->scope_depth = node->lhs->scope_depth;
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

Node* Evaluater::find_func(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
    for( auto&& i : (*it)->list ) {
      if( i && i->kind == NODE_FUNCTION && i->name == name ) {
        return i;
      }
    }
  }

  return nullptr;
}

