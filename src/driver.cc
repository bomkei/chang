#include "chang.h"

Driver::Driver() {

}

Driver::~Driver() {

}

bool Driver::parse_arguments(int argc, char** argv) {

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

  //view_token(tok);

  Parser parser(tok);

  alert;
  auto node = parser.parse();

  if( !global.entry_point ) {
    std::cout << "chang: doesn't exists function with named 'main'." << std::endl;
    exit(1);
  }

  Evaluater eval;
  alert;
  eval.evaluate(node);

  Interpreter runner;
  alert;
  std::cout << runner.run_node(global.entry_point->expr) << std::endl;

  return 0;
}