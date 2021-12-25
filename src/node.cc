#include "chang.h"

long Node::find_var(std::string_view const& name) {
  for( long i = 0; i < this->objects.size(); i++ ) {
    if( this->objects[i].name == name )
      return i;
  }

  return -1;
}

Node::Node(NodeKind kind)
  : kind(kind) {
}

Node::Node(NodeKind kind, Node* lhs, Node* rhs, Token* tok)
  : kind(kind), lhs(lhs), rhs(rhs), token(tok) {
}