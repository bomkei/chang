#include "token.h"
#include "object.h"
#include "node.h"
#include "error.h"
#include "debug.h"
#include "evaluater.h"

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
    case NODE_REFERENCE:
      return true;
  }

  return false;
}

Object* Evaluater::get_obj_addr(Node* node) {
  assert(node != nullptr);
  assert(is_lvalue(node));
  
  switch( node->kind ) {
    case NODE_VARIABLE: {
      return &node->get_var();
    }

    case NODE_REFERENCE: {
      return get_obj_addr(node->expr);
    }
  }

  error(ERR_VALUE_TYPE, node->token, "please report to developer.(code=0x4gbs8j2k)");
  exit(1);
}

void Evaluater::check_array(std::vector<Node*>::const_iterator ec_list_it, std::vector<Object>::const_iterator ec_obj_list_it, std::size_t depth, Node* arr) {
  auto arr_type = evaluate(arr);
  arr->objptr = &*ec_obj_list_it;

  if( *ec_list_it != nullptr ) {
    if( arr->kind != NODE_ARRAY ) {
      error(ERR_TYPE, arr->token, "expected empty array");
      return;
    }
    else if( !arr->list.empty() ) {
      error(ERR_TYPE, arr->token, "array must be empty");
      return;
    }

    arr->objtype.arr_depth = depth;
    return;
  }

  if( arr_type.arr_depth != depth ) {
    error(ERR_TYPE, arr->token, "type mismatch");
  }

  if( depth >= 2 && arr->kind == NODE_ARRAY ) {
    for( auto&& i : arr->list ) {
      check_array(ec_list_it + 1, ec_obj_list_it + 1, depth - 1, i);
    }
  }
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
