#include "chang.h"

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

Node* Parser::semicolon(Node* node) {
  switch( node->kind ) {
    case NODE_SCOPE:
    case NODE_FUNCTION:
      break;

    default:
      expect(";");
  }

  return node;
}