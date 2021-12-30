#include "error.h"
#include "debug.h"
#include "Utils.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Parser.h"
#include "Global.h"

Parser::Parser(Token* token)
  : token(token), consumed(nullptr) {

}

Node* Parser::parse() {
  auto node = new Node(NODE_SCOPE);

  while( check() ) {
    node->list.emplace_back(top());
  }

  return node;
}
