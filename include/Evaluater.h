#pragma once

#include <map>
#include <list>
#include "typefwd.h"

class Evaluater {
public:
  Evaluater();

  ObjectType primary(Node* node);
  ObjectType type(Node* node);
  ObjectType expr(Node* node);
  ObjectType stmt(Node* node);

  ObjectType evaluate(Node* node);

  static Evaluater* get_instance();

private:
  // check if branch to some codes.
  bool is_branchable(Node* node);

  // check if node is lvalue
  bool is_lvalue(Node* node);

  Object* get_obj_addr(Node* node);

  // check array
  void check_array(std::vector<Node*>::const_iterator ec_list_it, std::vector<Object>::const_iterator ec_obj_list_it, std::size_t depth, Node* arr);

  //  get all nodes which can be return value
  std::vector<Node*> get_return_values(Node* node);

  ObjectType must_integrated(Node* scope, std::vector<Node*> const& types);

  std::pair<Node*, std::size_t> find_var(std::string_view const& name);
  
  std::vector<ObjectType> eval_func_args(Node* func);
  std::vector<Node*> find_func(std::string_view const& name, std::vector<ObjectType> const& arg_types);

  bool in_main = false;
  std::size_t scope_depth = 0;
  std::list<Node*> scope_list;
  std::list<Node*> var_stmt_list;
  std::map<Object*, int> ref_counter;
  std::map<Object*, bool> initialized;
};
