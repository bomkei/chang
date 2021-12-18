#include "chang.h"

inline void view_token(Token* tok) {
  while( tok->kind != TOK_EOF ) {
    std::cout << tok->str << " ";
    tok = tok->next;
  }

  std::cout << std::endl;
}

int main(int argc, char** argv) {
  Global global;
  std::ifstream ifs("test.txt");
  
  for( std::string line; std::getline(ifs, line); ) {
    global.source += line + '\n';
  }

  Lexer lexer(global.source);

  auto tok = lexer.lex();

  view_token(tok);

  Parser parser(tok);

  auto node = parser.parse();

  Evaluater eval;
  eval.evaluate(node);

  Interpreter runner;
  std::cout << runner.run_node(node) << std::endl;


}