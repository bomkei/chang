#pragma once

#include <concepts>
#include <cassert>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <locale>
#include <map>

#include "debug.h"
#include "values.h"

#include "utils.h"
#include "token.h"
#include "object.h"
#include "node.h"

class Lexer {
public:
  Lexer(std::string const&);

  Token* lex();

private:
  char peek();
  bool check();
  bool match(std::string_view const& str);
  void pass_space();

  std::size_t pass_num();
  std::size_t pass_ident();
  std::size_t pass_string();

  std::string const& source;
  std::size_t position;
};

class Parser {
public:
  explicit Parser(Token*);

  Node* parse();

  Node* primary();
  Node* member();
  Node* unary();
  Node* mul();
  Node* add();
  Node* assign();
  Node* expr();
  Node* top();

private:
  void next();
  bool check();
  bool consume(char const*);
  void expect(char const*, bool = true);
  void expect_ident();

  Node* expect_argument();
  Node* expect_type();

  bool is_need_semicolon(Node* node);

  Token* token;
  Token* consumed;

  std::vector<Node*> structs;
};

class Interpreter {
  using EcObjIt = std::vector<Object>::const_reverse_iterator;

public:
  Interpreter();

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
