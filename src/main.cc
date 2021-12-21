#include "chang.h"

inline void view_token(Token* tok) {
  while( tok->kind != TOK_EOF ) {
    std::cout << tok->str << " ";
    tok = tok->next;
  }

  std::cout << std::endl;
}

int main(int argc, char** argv) {
  

}