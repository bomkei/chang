#pragma once

enum NodeKind {
  NODE_EXPR,
  NODE_ASSIGN,

  NODE_VALUE,
  NODE_VARIABLE,

  NODE_ARRAY,
  NODE_INDEX_REF,

  NODE_GET_ADDR,
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

  // return
  NODE_RETURN,

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
    Token* token;
    Node* item;
  };

  struct Enum {
    std::string_view name;
    Node* value_type;
  };

  NodeKind kind;
  Token* token;
  Object obj;
  //Node* lhs;
  //Node* rhs;

  std::vector<Node*> list;
  std::vector<Object> objects;
  std::vector<ExprPair> expr_list;
  std::vector<Enum> enum_list;

  std::string_view name;
  Node* expr;

  ObjectType objtype;
  std::vector<Node*> elemcount_list;

  union {
    bool is_make_array;
    bool is_allowed_let;
    bool is_allowed_empty_array;
    bool is_allowed_return;

    struct {
      Node* item = nullptr;
      Node* member;
    };

    struct {
      Node* func;
      BuiltinFunc const* builtin;
    };

    struct {
      Node* if_true;
      Node* if_else;
    };

    struct {
      bool is_reference;
      bool evaluated;
      Node* type;
    };

    struct {
      long var_index;
      Node* var_scope;
      Object const* elemcount;
    };
  };

  // these are used in location specific statements.
  // if true, it is meaning can be placed to there.

  // is allowed empty arr

  // built-in func

  Object& get_var() const {
    sizeof(Node);
    return var_scope->objects[var_index];
  }

  long find_var(std::string_view const& name) {
    for( long i = 0; i < objects.size(); i++ ) {
      if( objects[i].name == name )
        return i;
    }

    return -1;
  }

  Node(NodeKind kind)
    : kind(kind) {
  }

  // Node(NodeKind kind, Node* lhs, Node* rhs, Token* tok = nullptr)
  //   : kind(kind), lhs(lhs), rhs(rhs), token(tok) {
  // }
};