#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Lexer.h"

static char const* reserved_tokens[] = {
  "continue",
  "return",
  "false",
  "while",
  "break",
  "true",
  "loop",
  "none",
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

  pass_space();

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

      for( auto&& i : reserved_tokens ) {
        if( i == cur->str ) {
          cur->kind = TOK_RESERVED;
          break;
        }
      }
    }

    // string
    else if( ch == '"' ) {
      cur->kind = TOK_STRING;

      cur->pos = position += 1;
      cur->str = { str + 1, pass_string() };
    }

    else {
      for( std::string_view&& op : reserved_tokens ) {
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

