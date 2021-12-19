#include "chang.h"

Node* Parser::primary() {
  
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

    next();
    expect("(");

    // todo: read args
    expect(")");

    expect("{", false);
    node->
    

    return node;
  }

  error(ERR_PARSE, token, "expected function declare");
  exit(1);
}