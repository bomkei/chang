#include "chang.h"

inline void view_token(Token* tok) {
  while( tok->kind != TOK_EOF ) {
    std::cout << tok->str << " ";
    tok = tok->next;
  }

  std::cout << std::endl;
}

int main(int argc, char** argv) {
  try {
    return Driver().main(argc, argv);
  }
  catch( std::exception const& ex ) {
    std::cout
      << "unhandled exception has occurred, please report to developer." << std::endl
      << "what(): " << ex.what() << std::endl;
  }
}