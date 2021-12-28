#pragma once

#include <list>

struct Node;
struct Object;

class Interpreter {
  using EcObjIt = std::vector<Object>::const_reverse_iterator;
    // todo: write explanation

public:
  Interpreter();

  void expr_obj(ExprKind kind, Object& a, Object& b);
  bool compare_obj(ExprKind kind, Object& a, Object& b);

  Object run_primary(Node* node);
  Object run_expr(Node* node);
  Object run_stmt(Node* node);

  Object run_node(Node* node);
  Object& run_lvalue(Node* node);

  Object construct_array(ObjectKind kind, EcObjIt end, EcObjIt it);

  static Interpreter* get_instance();

  static void add(Object& obj, Object& val);
  static void sub(Object& obj, Object& val);
  static void mul(Object& obj, Object& val);
  static void div(Object& obj, Object& val);

private:
  std::list<Node*> var_stmt_list;
};
