#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Lexer.h"

char Lexer::peek(){ 
  return source[position];
}

bool Lexer::check() {
  return position < source.length();
}

bool Lexer::match(std::string_view const& str) {
  return position + str.length() <= source.length() && source.substr(position, str.length()) == str;
}

void Lexer::pass_space() {
  while( check() && peek() <= ' ' ) {
    position += 1;
  }
}

std::size_t Lexer::pass_num() {
  std::size_t len = 0;

  while( check() && isdigit(peek()) ) {
    len += 1;
    position += 1;
  }

  return len;
}

std::size_t Lexer::pass_ident() {
  std::size_t len = 0;

  while( check() && (isalnum(peek()) || peek() == '_') ) {
    len += 1;
    position += 1;
  }

  return len;
}

std::size_t Lexer::pass_string() {
  std::size_t len = 0;

  while( check() && peek() != '"' ) {
    len++, position++;
  }

  position++;
  return len;
}