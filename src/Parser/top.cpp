#include "error.h"
#include "debug.h"
#include "Utils.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Parser.h"
#include "Global.h"

Node* Parser::top() {
  if( consume("fn") ) {
    auto node = new Node(NODE_FUNCTION);
    node->token = consumed;

    expect_ident();
    node->name = token->str;

    auto is_main = node->name == "main";

    if( is_main ) {
      Global::get_instance()->entry_point = node;

      node->type = new Node(NODE_TYPE);
      node->type->name = "int";
      node->type->token = node->token;
    }

    next();
    expect("(");

    if( !consume(")") ) {
      do {
        node->list.emplace_back(expect_argument());
      } while( consume(",") );
      expect(")");
    }

    if( consume("->") ) {
      if( is_main ) {
        error(ERR_UNEXPECTED, consumed, "cannot specify return type of 'main' function, already specified with int implicity.");
        exit(1);
      }

      node->type = expect_type();
    }

    expect("{", false);
    node->expr = expr();

    if( is_main ) {
      auto zero = Global::get_instance()->main_zero = node->expr->list.emplace_back(new Node(NODE_VALUE));

      zero->token = node->token;
      zero->obj.type = zero->objtype = OBJ_INT;
      zero->evaluated = true;
    }

    return node;
  }

  error(ERR_PARSE, token, "expected function declare");
  exit(1);
}