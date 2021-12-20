#include "chang.h"

Node* Parser::primary() {
  
  if( consume("{") ) {
    auto node = new Node(NODE_SCOPE);

    node->token = consumed;

    if( consume("}") ) {
      return node;
    }

    while( check() ) {
      node->list.emplace_back(expr());

      if( consume(";") ) {
        if( token->str == "}" ) {
          error(ERR_UNEXPECTED, token, "unexpected character '}'");
          error(ERR_NOTE, consumed, "semicolon is not needed at last of scope. maybe did you forget remove it?");
          exit(1);
        }

        continue;
      }

      expect("}");
      break;
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
  }

  error(ERR_SYNTAX, token, "syntax error");
  exit(1);
}

Node* Parser::mul() {
  auto node = primary();

  while( check() ) {
    if( consume("*") ) node = new Node(NODE_MUL, node, primary(), consumed);
    else if( consume("/") ) node = new Node(NODE_DIV, node, primary(), consumed);
    else break;
  }

  return node;
}

Node* Parser::add() {
  auto node = mul();

  while( check() ) {
    if( consume("+") ) node = new Node(NODE_ADD, node, mul(), consumed);
    else if( consume("-") ) node = new Node(NODE_SUB, node, mul(), consumed);
    else break;
  }

  return node;
}

Node* Parser::expr() {
  return add();
}

Node* Parser::top() {
  if( consume("fn") ) {
    auto node = new Node(NODE_FUNCTION);

    expect_ident();
    node->name = token->str;

    if( node->name == "main" ) {
      Global::get_instance()->entry_point = node;
    }

    next();
    expect("(");

    // todo: read args
    expect(")");

    expect("{", false);
    node->code = expr();
    

    return node;
  }

  error(ERR_PARSE, token, "expected function declare");
  exit(1);
}