#pragma once

#include <list>

struct Node;
struct Object;

class Interpreter {
  using EcObjIt = std::vector<Object>::const_reverse_iterator;
    // todo: write explanation

public:
  Interpreter();

  Object run_primary(Node* node);
  Object run_expr(Node* node);
  Object run_stmt(Node* node);

  Object run_node(Node* node);
  Object& run_lvalue(Node* node);

  Object construct_array(ObjectKind kind, EcObjIt end, EcObjIt it);
  void fit_array_length(std::vector<Object>::const_iterator const& ec_obj_it, Object& arr);

  static Interpreter* get_instance();

  static void add(Object& obj, Object& val);
  static void sub(Object& obj, Object& val);
  static void mul(Object& obj, Object& val);
  static void div(Object& obj, Object& val);

private:
  std::list<Node*> var_stmt_list;
};
