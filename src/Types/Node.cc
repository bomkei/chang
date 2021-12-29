#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "debug.h"

std::string Node::to_string(Node* node) {
  if( !node )
    return "";
  
  switch( node->kind ) {
    case NODE_VALUE:
      return node->obj.to_string();
  }

  return "";
}