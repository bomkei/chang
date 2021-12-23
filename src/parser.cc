#include "chang.h"

Parser::Parser(Token* token)
  : token(token), consumed(nullptr) {

}

Node* Parser::parse() {
  auto node = new Node(NODE_SCOPE);

  while( check() ) {
    node->list.emplace_back(top());
  }

  return node;
}

void Parser::next() {
  token = token->next;
}

bool Parser::check() {
  return token->kind != TOK_EOF;
}

bool Parser::consume(char const* str) {
  if( token->str == str ) {
    consumed = token;
    next();
    return true;
  }

  return false;
}

void Parser::expect(char const* str, bool step) {
  if( step ? !consume(str) : token->str != str ) {
    error(ERR_EXPECTED, token, "expected '%s'", str);
    exit(1);
  }
}

void Parser::expect_ident() {
  if( token->kind != TOK_IDENT ) {
    error(ERR_EXPECTED, token, "expected identifier");
    exit(1);
  }
}

Node* Parser::expect_argument() {
  Node* node = new Node(NODE_ARGUMENT);

  expect_ident();
  node->token = token;
  node->name = token->str;

  next();
  expect(":");

  node->type = expect_type();

  return node;
}

Node* Parser::expect_type() {
  Node* node = new Node(NODE_TYPE);

  expect_ident();
  node->token = token;
  node->name = token->str;

  next();

  if( consume("<") ) {
    do {
      node->list.emplace_back(expect_type());
    } while( consume(",") );

    if( token->str == ">>" ) {
      token->str = ">";
      token->insert(">");
    }

    expect(">");
  }

  while( consume("[]") ) {
    node->arr_depth++;
  }

  return node;
}

bool Parser::is_need_semicolon(Node* node) {
  switch( node->kind ) {
    case NODE_VAR:
      return true;
  }

  return false;
}

Node* Parser::primary() {

  if( consume("(") ) {
    auto e = expr();
    expect(")");
    return e;
  }

  if( consume("{") ) {
    auto node = new Node(NODE_SCOPE);

    node->token = consumed;

    if( consume("}") ) {
      return node;
    }

    while( check() ) {
      auto item = expr();
      node->list.emplace_back(item);

      if( consume(";") ) {
        if( consume("}") ) {
          node->list.emplace_back(nullptr);
          break;
        }

        continue;
      }
      else if( token->str != "}" ) {
        switch( item->kind ) {
          case NODE_IF:
            continue;
        }
      }

      expect("}");

      if( is_need_semicolon(item) ) {
        error(ERR_EXPECTED, consumed, "expected ';' , because previous statement is can't be return value.");
      }

      break;
    }

    return node;
  }

  if( consume("if") ) {
    auto node = new Node(NODE_IF);
    node->token = consumed;

    node->expr = expr();

    expect("{", false);
    node->if_true = expr();

    if( consume("else") ) {
      if( token->str != "if" ) {
        expect("{", false);
      }

      node->if_else = expr();
    }
    else {
      node->if_else = nullptr;
    }

    return node;
  }

  auto node = new Node(NODE_VALUE);

  node->token = token;

  switch( token->kind ) {
    case TOK_INT:
      node->obj.type = OBJ_INT;
      node->obj.v_int = atoi(token->str.cbegin());
      break;
    
    case TOK_CHAR:
      node->obj.type = OBJ_CHAR;
      node->obj.v_char = *token->str.cbegin();
      break;
    
    case TOK_FLOAT:
      node->obj.type = OBJ_FLOAT;
      node->obj.v_float = atof(token->str.cbegin());
      break;
    
    case TOK_STRING:
      node->obj.type = OBJ_STRING;
      node->obj.v_str = Utils::String::to_utf16(token->str);
      break;
    
    case TOK_IDENT: {
      node->kind = NODE_VARIABLE;
      node->token = token;
      node->name = token->str;

      next();

      if( consume("(") ) {
        node->kind = NODE_CALLFUNC;

        if( consume(")") ) {
          return node;
        }

        do {
          node->list.emplace_back(expr());
        } while( consume(",") );

        expect(")");
      }

      break;
    }

    case TOK_RESERVED: {
      node->obj.type = OBJ_BOOL;

      if( token->str == "true" )
        node->obj.v_bool = true;
      else if( token->str == "false" )
        node->obj.v_bool = false;
      else
        goto unk;
      
      break;
    }
  }

  next();
  return node;

unk:;
  error(ERR_SYNTAX, token, "syntax error");
  exit(1);
}

Node* Parser::member() {
  auto node = primary();

  while( consume(".") ) {
    auto x = primary();

    if( x->kind == NODE_CALLFUNC ) {
      x->list.insert(x->list.begin(), node);
      node = x;
    }
    else {
      x = new Node(NODE_MEMBER_ACCESS, node, x, consumed);
    }
  }

  return node;
}

Node* Parser::mul() {
  auto node = member();

  if( token->str == "*" || token->str == "/" ) {
    auto expr = new Node(NODE_EXPR);
    expr->expr = node;
    expr->token = token;

    while( check() ) {
      ExprKind kind;

      if( consume("*") )
        kind = EXPR_MUL;
      else if( consume("/") )
        kind = EXPR_DIV;
      else
        break;

      expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, member() });
    }

    return expr;
  }

  return node;
}

Node* Parser::add() {
  auto node = mul();

  if( token->str == "+" || token->str == "-" ) {
    auto expr = new Node(NODE_EXPR);
    expr->expr = node;
    expr->token = token;

    while( check() ) {
      ExprKind kind;

      if( consume("+") )
        kind = EXPR_ADD;
      else if( consume("-") )
        kind = EXPR_SUB;
      else
        break;
      
      expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, mul() });
    }

    return expr;
  }

  return node;
}

Node* Parser::expr() {
  if( consume("var") ) {
    alert;
    
    auto node = new Node(NODE_VAR);
    node->token = consumed;

    expect_ident();
    node->name = token->str;

    next();

    if( consume(":") ) {
      node->type = expect_type();
    }

    if( consume("=") ) {
      node->expr = expr();
    }

    if( !node->type && !node->expr ) {
      error(ERR_TYPE, node->token, "cannot infer a type of variable");
    }
    
    alert;
    return node;
  }

  return add();
}

Node* Parser::top() {
  if( consume("fn") ) {
    auto node = new Node(NODE_FUNCTION);
    node->token = consumed;

    expect_ident();
    node->name = token->str;

    auto is_main = node->name == "main";

    if( is_main ) {
      Global::get_instance()->entry_point = node;

      node->type = new Node(NODE_TYPE);
      node->type->name = "int";
      node->type->token = node->token;
    }

    next();
    expect("(");

    if( !consume(")") ) {
      do {
        node->list.emplace_back(expect_argument());
      } while( consume(",") );
      expect(")");
    }

    if( consume("->") ) {
      if( is_main ) {
        error(ERR_UNEXPECTED, consumed, "cannot specify return type of 'main' function, already specified as int implicity.");
        exit(1);
      }

      node->type = expect_type();
    }

    expect("{", false);
    node->expr = expr();

    if( is_main ) {
      alert;
      node->expr->list.emplace_back(new Node(NODE_VALUE))->obj.type = OBJ_INT;
    }

    return node;
  }

  error(ERR_PARSE, token, "expected function declare");
  exit(1);
}