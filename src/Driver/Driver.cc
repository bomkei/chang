#include <iostream>
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Driver.h"
#include "Global.h"
#include "Lexer.h"
#include "Parser.h"
#include "Evaluater.h"
#include "Interpreter.h"

inline void view_token(Token* tok) {
  while( tok->kind != TOK_EOF ) {
    std::cout << tok->str << " ";
    tok = tok->next;
  }

  std::cout << std::endl;
}

Driver::Driver() {

}

Driver::~Driver() {

}

bool Driver::parse_arguments(int argc, char** argv) {
  return true;
}

int Driver::main(int argc, char** argv) {
  global.file_path = "test.txt";

  std::ifstream ifs(global.file_path);
  
  for( std::string line; std::getline(ifs, line); ) {
    global.source += line + '\n';
  }

  Lexer lexer(global.source);

  alert;
  auto tok = lexer.lex();

  if( global.is_error_occurred )
    return 0;

  Parser parser(tok);

  alert;
  auto node = parser.parse();

  global.top_node = node;

  if( global.is_error_occurred )
    return 0;

  Evaluater eval;

  alert;
  eval.evaluate(node);

  if( !global.entry_point ) {
    std::cout << "entry point function 'main' is not defined." << std::endl;
    exit(1);
  }

  if( global.is_error_occurred )
    return 0;

  Interpreter runner;
  
  alert;
  runner.run_node(global.entry_point->expr);

  return 0;
}