#pragma once

#include <string>

class Lexer {
public:
  Lexer(std::string const&);

  Token* lex();

private:
  char peek();
  bool check();
  bool match(std::string_view const& str);
  void pass_space();

  std::size_t pass_num();
  std::size_t pass_ident();
  std::size_t pass_string();

  std::string const& source;
  std::size_t position;
};
