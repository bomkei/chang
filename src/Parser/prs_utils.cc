#include "error.h"
#include "debug.h"
#include "Utils.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Parser.h"
#include "Global.h"

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
    if( isalpha(token->str[0]) || token->str[0] == '_' )
      return;

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

  while( consume("[") ) {
    if( consume("]") ) {
      node->elemcount_list.emplace_back(nullptr);
    }
    else {
      node->elemcount_list.emplace_back(expr());
      expect("]");
    }
  }

  if( consume("&") ) {
    node->is_reference = true;
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
