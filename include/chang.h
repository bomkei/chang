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

class Evaluater {
public:
  Evaluater();

  ObjectType evaluate(Node* node);

  static Evaluater* get_instance();

private:
  bool is_branchable(Node* node);

  // check array
  void check_array(std::vector<Node*>::const_iterator ec_list_it, std::vector<Object>::const_iterator ec_obj_list_it, std::size_t depth, Node* arr);

  //  get all nodes which can be return value
  std::vector<Node*> get_return_values(Node* node);

  ObjectType must_integrated(Node* node);

  std::pair<Node*, std::size_t> find_var(std::string_view const& name);
  
  std::vector<ObjectType> eval_func_args(Node* func);
  std::vector<Node*> find_func(std::string_view const& name, std::vector<ObjectType> const& arg_types);

  bool in_main = false;
  std::list<Node*> scope_list;
  std::list<Node*> var_stmt_list;
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

struct BuiltinFunc {
  using FuncPointer = Object(*)(std::vector<Object>&);

  char const* name;
  bool arg_free;
  std::vector<ObjectType> arg_types;
  ObjectType ret_type;
  FuncPointer func;

  static std::vector<BuiltinFunc> const& get_list();

private:
  BuiltinFunc(char const*, bool, std::vector<ObjectType>, ObjectType, FuncPointer);
};

enum ErrorKind {
  ERR_LEX,
  ERR_PARSE,
  ERR_SYNTAX,
  ERR_EXPECTED,
  ERR_UNEXPECTED,
  ERR_UNDEFINED,
  ERR_MULTIPLE_DEFINED,
  ERR_MANY_CANDIDATES,
  ERR_RETURN,
  ERR_LOCATION,
  ERR_ARGUMENT,
  ERR_TYPE,
  ERR_WARN,
  ERR_NOTE
};

struct Global {
  std::string source;
  std::string file_path;
  Node* entry_point = nullptr;
  Node* top_node = nullptr;

  bool is_error_occurred = false;

  static Global* get_instance();

private:
  Global();
  friend class Driver;
};

class Driver {
public:
  Driver();
  ~Driver();

  bool parse_arguments(int argc, char** argv);

  int main(int argc, char** argv);

private:
  Global global;
};

void error(ErrorKind kind, Token* token, char const* fmt, ...);
