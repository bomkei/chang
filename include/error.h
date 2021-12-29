#pragma once

struct Token;

enum ErrorKind {
  ERR_LEX,
  ERR_PARSE,
  ERR_SYNTAX,
  ERR_OPERATOR,
  ERR_EXPECTED,
  ERR_UNEXPECTED,
  ERR_UNDEFINED,
  ERR_MULTIPLE_DEFINED,
  ERR_CANDIDATES,
  ERR_TYPE,
  ERR_VALUE_TYPE,
  ERR_REFERENCE,
  ERR_LIFE_SPAN,
  ERR_UNINITIALIZED,
  ERR_RETURN,
  ERR_MAIN_FUNC,
  ERR_LOCATION,
  ERR_ARGUMENT,
  ERR_OUT_OF_RANGE,
  ERR_WARN,
  ERR_NOTE
};

void errortext(char const* fmt, ...);

void error(ErrorKind kind, Token* token, std::string const& msg);
void error_node(ErrorKind kind, Node* node, std::string const& msg);

