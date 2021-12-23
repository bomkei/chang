#pragma

#include <concepts>
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <map>

#define  __DEBUG__  1

#if __DEBUG__
  #define alert fprintf(stderr,"\t#alert at %s:%d\n",__FILE__,__LINE__)
  #define debug(...) __VA_ARGS__
  #define dbgprint(...) {fprintf(stderr,__VA_ARGS__);fflush(stderr);}
  #define loop(n,...)  for(int i=0;i<n;i++){__VA_ARGS__}
  #define print_line(...) {fprintf(stderr,"\n------------------\n%s\tline %6d| %s\n\n",__FILE__,__LINE__,#__VA_ARGS__); __VA_ARGS__ }
#else
  #define alert 0
  #define debug(...) 0;
  #define dbgprint(...) 0
  #define loop(...) 0
  #define print_line(...)  __VA_ARGS__
#endif

#define  COL_DEFAULT   "\033[37m"
#define  COL_WHITE     "\033[97m"
#define  COL_SKYBLUE   "\033[96m"
#define  COL_PINK      "\033[95m"
#define  COL_BLUE      "\033[94m"
#define  COL_YELLOW    "\033[93m"
#define  COL_GREEN     "\033[92m"
#define  COL_RED       "\033[91m"
#define  COL_GRAY      "\033[90m"
#define  COL_CYAN      "\033[36m"
#define  COL_MAGENTA   "\033[35m"

namespace Utils {
  inline auto str(std::string_view const& str) {
    static char buf[0x1000];
    memcpy(buf, str.cbegin(), str.length());
    buf[str.length()] = 0;
    return buf;
  }
}

template <class T>
concept HaveMethodForString = requires (T const& x) {
  x.to_string();
};

template <HaveMethodForString T>
auto& operator << (std::ostream& ost, T const& x) {
  return ost << x.to_string();
}

enum TokenKind {
  TOK_INT,
  TOK_CHAR,
  TOK_FLOAT,
  TOK_STRING,
  TOK_IDENT,
  TOK_RESERVED,
  TOK_EOF
};

struct Token {
  TokenKind kind;
  std::string_view str;
  Token* back;
  Token* next;
  std::size_t pos;
  
  Token() { }

  Token(TokenKind kind, Token* back, std::size_t pos)
    : kind(kind), back(back), pos(pos) {
    if( back ) {
      back->next = this;
    }
  }
};

enum ObjectKind {
  OBJ_INT,
  OBJ_NONE
};

struct ObjectType {
  ObjectKind kind;
  
  ObjectType(ObjectKind kind = OBJ_NONE)
    : kind(kind) {
  }

  bool equals(ObjectType const&) const;
};

struct Object {
  ObjectType type;
  std::string_view name;

  long v_int;

  std::string to_string() const;
};

enum NodeKind {
  NODE_EXPR,
  NODE_ASSIGN,

  NODE_VALUE,
  NODE_VARIABLE,
  NODE_MEMBER_ACCESS,

  // argument
  //   name = argument name
  //   type = type
  NODE_ARGUMENT,

  // type
  //   name = type name
  NODE_TYPE,

  // call Func
  //   name = function name
  //   list = arguments
  NODE_CALLFUNC,

  // variable Declaration
  //   name = variable name
  //   expr = initializer expr
  NODE_VAR,

  // if statement
  NODE_IF,

  // scope
  //   list = elements
  NODE_SCOPE,

  // function
  //   name = function name
  //   list = arguments (NODE_ARGUMENT)
  //   expr = code of function
  NODE_FUNCTION,

  // enum
  //   name       = name
  //   enum_list  = elements
  NODE_ENUM,
};

enum ExprKind {
  EXPR_ADD,
  EXPR_SUB,
  EXPR_MUL,
  EXPR_DIV,
};

struct BuiltinFunc;
struct Node {
  struct ExprPair {
    ExprKind kind;
    Node* item;
  };

  struct Enum {
    std::string_view name;
    Node* value_type;
  };

  NodeKind kind;
  Token* token;
  Node* lhs;
  Node* rhs;
  Object obj;
  std::vector<Node*> list;
  std::vector<Object> objects;
  std::vector<ExprPair> expr_list;
  std::vector<Enum> enum_list;

  std::string_view name;
  Node* type;
  Node* expr;

  Node* if_true;
  Node* if_else;

  bool evaluated = false;
  ObjectType objtype;

  // variable
  Node* var_scope;
  long var_index = -1;

  // if self is NODE_VAR, this is can use to check if placed self on allowed area.
  // example for, root in scope.
  bool is_allowed_let = false;

  // built-in func
  BuiltinFunc const* builtin = nullptr;

  long find_var(std::string_view const& name);

  Node(NodeKind kind);
  Node(NodeKind kind, Node* lhs, Node* rhs, Token* tok = nullptr);
};

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

  std::string const& source;
  std::size_t position;
};

class Parser {
public:
  explicit Parser(Token*);

  Node* parse();

  Node* primary();
  Node* member();
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
  ObjectType evaluate(Node* node);

private:
  bool is_branchable(Node* node);

  //  get all nodes which can be return value
  std::vector<Node*> get_return_values(Node* node);

  // check if all nodes which can be
  //  return value has integrated as same types.
  //
  // return:
  //   [true, nullptr]  = integrated
  //   [false, <node>]  = not integrated
  std::pair<bool, Node*> is_integrated(Node* node);

  std::pair<Node*, std::size_t> find_var(std::string_view const& name);
  Node* find_func(std::string_view const& name);

  std::list<Node*> scope_list;
};

class Interpreter {
public:
  Object run_node(Node* node);
  Object& run_lvalue(Node* node);

  static void add(Object& obj, Object& val);
  static void sub(Object& obj, Object& val);
  static void mul(Object& obj, Object& val);
  static void div(Object& obj, Object& val);

private:

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

  bool was_error_occurred = false;

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
