#pragma once

#include <string>

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
  std::size_t pos;
  Token* back;
  Token* next;
  
  Token() { }

  Token(TokenKind kind, Token* back, std::size_t pos)
    : kind(kind), back(back), pos(pos) {
    if( back ) {
      back->next = this;
    }
  }

  Token* insert(std::string_view const& str, TokenKind kind = TOK_RESERVED) {
    auto tok = new Token;

    tok->kind = kind;
    tok->str = str;
    tok->back = this;
    tok->next = this->next;

    this->next = tok;
    return tok;
  }

  static Token* from_string(std::string_view const& str, TokenKind kind = TOK_IDENT) {
    auto tok = new Token;

    tok->kind = kind;
    tok->str = str;

    return tok;
  }
};
