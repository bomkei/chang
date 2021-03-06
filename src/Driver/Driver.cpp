#include <iostream>
#include <fstream>
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Driver.h"
#include "Global.h"
#include "Utils.h"
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

  auto tok = lexer.lex();

  if( global.is_error_occurred )
    return 0;

  Parser parser(tok);

  auto node = parser.parse();

  global.top_node = node;

  if( global.is_error_occurred )
    return 0;

  Evaluater eval;

  eval.evaluate(node);

  if( !global.entry_point ) {
    std::cout << "entry point function 'main' is not defined." << std::endl;
    exit(1);
  }

  if( global.is_error_occurred )
    return 0;

  Interpreter runner;
  
  if( !global.entry_point->expr->objects.empty() ) {
    for( int i = 1; i < argc; i++ ) {
      auto& arg = global.entry_point->expr->objects[0].list.emplace_back();

      arg.type = OBJ_STRING;
      arg.v_str = Utils::String::to_utf16(argv[i]);
    }
  }

  runner.run_node(global.entry_point->expr);

  return 0;
}