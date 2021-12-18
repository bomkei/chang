#include "chang.h"

Parser::Parser(Token* token)
  : token(token), consumed(nullptr) {

}

Node* Parser::parse() {
  return expr();
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