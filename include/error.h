#pragma once

struct Token;

enum ErrorKind {
  ERR_LEX,
  ERR_PARSE,
  ERR_SYNTAX,
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
  ERR_LOCATION,
  ERR_ARGUMENT,
  ERR_WARN,
  ERR_NOTE
};

void error(ErrorKind kind, Token* token, char const* fmt, ...);
