#include "chang.h"

static char const* operator_token_list[] = {
  "continue",
  "return",
  "false",
  "while",
  "break",
  "true",
  "loop",
  "var",
  "for",
  "if",
  "do",
  "fn",
  "...", ">>=", "<<=", "<=>", "->", "<-", ">=", "<=", ">>", "<<", "==", "!=", "+=",
  "-=", "*=", "/=", "%=", "&=", "|=", "&&", "||", "::", "!", "=", ".", ",", "?",
  "~", ">", "<", "%", "&", "^", "|", "(", ")", "[", "]", "{", "}", ";", ":", "+", "-", "*", "/", "@"
};

Lexer::Lexer(std::string const& src)
  : source(src), position(0) {

}

Token* Lexer::lex() {
  Token top;
  Token* cur = &top;

  while( check() ) {
    auto ch = peek();
    auto pos = position;
    auto str = source.c_str() + pos;

    cur = new Token(TOK_INT, cur, pos);

    // numeric
    if( isdigit(ch) ) {
      cur->str = { str, pass_num() };

      if( peek() == '.' ) {
        position += 1;

        if( !isdigit(peek()) ) {
          position -= 1;
          continue;
        }

        cur->kind = TOK_FLOAT;
        cur->str = { str, pass_num() + cur->str.length() + 1 };
      }
    }

    // identifier
    else if( isalpha(ch) || ch == '_' ) {
      cur->kind = TOK_IDENT;
      cur->str = { str, pass_ident() };
    }

    // string
    else if( ch == '"' ) {
      cur->kind = TOK_STRING;

      cur->pos = position += 1;
      cur->str = { str + 1, pass_string() };
    }

    else {
      for( std::string_view&& op : operator_token_list ) {
        if( match(op) ) {
          cur->kind = TOK_RESERVED;
          cur->str = op;
          position += op.length();
          goto such_op;
        }
      }

      error(ERR_LEX, cur, "unknown token");
      exit(1);
    such_op:;
    }

    pass_space();
  }

  cur = new Token(TOK_EOF, cur, position);
  return top.next;
}

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