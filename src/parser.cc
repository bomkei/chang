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
        error(ERR_NOTE, item->token, "this is return nothing");
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

  switch( token->kind ) {
    case TOK_INT: {
      auto node = new Node(NODE_VALUE);

      node->token = token;
      node->obj.type = OBJ_INT;
      node->obj.v_int = atoi(token->str.cbegin());

      next();
      return node;
    }

    case TOK_IDENT: {
      auto node = new Node(NODE_VARIABLE);

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

      return node;
    }
  }

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

      expr->expr_list.emplace_back(Node::ExprPair{ kind, member() });
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
      
      expr->expr_list.emplace_back(Node::ExprPair{ kind, mul() });
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

    if( node->name == "main" ) {
      Global::get_instance()->entry_point = node;
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
      if( node->name == "main" ) {
        error(ERR_UNEXPECTED, consumed, "cannot specify return type of 'main' function, already specified as int automatically.");
        exit(1);
      }

      node->type = expect_type();
    }

    expect("{", false);
    node->expr = expr();

    return node;
  }

  error(ERR_PARSE, token, "expected function declare");
  exit(1);
}