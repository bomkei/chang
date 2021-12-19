#pragma

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <concepts>

#define  __DEBUG__  1

#if __DEBUG__
  #define alart fprintf(stderr,"\t#alart at %s:%d\n",__FILE__,__LINE__)
  #define debug(...) __VA_ARGS__
  #define dbgprint(...) {fprintf(stderr,__VA_ARGS__);fflush(stderr);}
  #define loop(n,...)  for(int i=0;i<n;i++){__VA_ARGS__}
  #define print_line(...) {fprintf(stderr,"\n------------------\n%s\tline %6d| %s\n\n",__FILE__,__LINE__,#__VA_ARGS__); __VA_ARGS__ }
#else
  #define alart 0
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

  long v_int;

  std::string to_string() const;
};

enum NodeKind {
  NODE_ADD,
  NODE_SUB,
  NODE_MUL,
  NODE_DIV,
  NODE_ASSIGN,

  NODE_VAR,
  NODE_SCOPE,

  // Type
  //  name = type name
  //  
  NODE_TYPE,

  // Function
  //  name = function name
  //  list = arguments (NODE_ARGUMENT)
  //  code = code
  NODE_FUNCTION,

  // Argument
  //  name = argument name
  //  type = type
  NODE_ARGUMENT,

  NODE_VALUE,
};

struct Node {
  NodeKind kind;
  Token* token;
  Node* lhs;
  Node* rhs;
  Object obj;
  std::vector<Node*> list;
  std::vector<Object> objects;

  std::string_view name;
  Node* type;
  Node* code;

  bool evaluated = false;

  Node(NodeKind kind)
    : kind(kind) {

  }

  Node(NodeKind kind, Node* lhs, Node* rhs, Token* tok = nullptr)
    : kind(kind), lhs(lhs), rhs(rhs), token(tok) {

  }
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

  Node* expect_argument();
  Node* expect_type();

  Node* primary();
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

  Node* semicolon(Node* node);

  Token* token;
  Token* consumed;

  std::vector<Node*> parsed_types;
};

class Evaluater {
public:
  ObjectType evaluate(Node* node);

private:

};

class Interpreter {
public:
  Object run_node(Node* node);

private:

};

enum ErrorKind {
  ERR_LEX,
  ERR_PARSE,
  ERR_SYNTAX,
  ERR_EXPECTED,
  ERR_TYPE
};

struct Global {
  std::string source;
  std::string file_path;

  Global();

  static Global* get_instance();
};


void error(ErrorKind kind, Token* token, char const* fmt, ...);
