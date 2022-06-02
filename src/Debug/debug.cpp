#include "Node.h"

namespace {
  long tab = 0;
}

// !! dont use in multi thread !!
std::string node_to_string(Node* node) {
  if( !node ) {
    return "<null>";
  }

  switch( node->kind ) {
    case NODE_VALUE:
      return node->obj.to_string();
    
    case NODE_VARIABLE:
      return std::string(node->name);
    
    case NODE_TYPE: {
      std::string s{ node->name };

      if( !node->list.empty() ) {
        s += "<";

        for( std::size_t i = 0; i < node->list.size(); i++ ) {
          s += node_to_string(node->list[i]);
          if( i < node->list.size() - 1 ) s += ", ";
        }

        s += ">";
      }

      for( auto&& i : node->elemcount_list ) {
        s += "[" + node_to_string(i) + "]";
      }

      return s;
    }

    case NODE_ARGUMENT: {
      return std::string(node->name) + ": " + node_to_string(node->type);
    }

    case NODE_FUNCTION: {
      std::string s = "fn " + std::string(node->name) + "(";


    }
  }

  return "";
}