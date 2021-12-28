#include "error.h"
#include "debug.h"
#include "Utils.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Parser.h"
#include "Global.h"

Node* Parser::mul() {
  auto node = unary();

  if( token->str == "*" || token->str == "/" ) {
    auto expr = new Node(NODE_EXPR);
    expr->expr = node;
    expr->token = token;

    while( check() ) {
      ExprKind kind;

      if( consume("*") )
        kind = EXPR_MUL;
      else if( consume("/") )
        kind = EXPR_DIV;
      else
        break;

      expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, unary() });
    }

    return expr;
  }

  return node;
}

Node* Parser::add() {
  auto node = mul();

  if( token->str == "+" || token->str == "-" ) {
    auto expr = new Node(NODE_EXPR);
    expr->expr = node;
    expr->token = token;

    while( check() ) {
      ExprKind kind;

      if( consume("+") )
        kind = EXPR_ADD;
      else if( consume("-") )
        kind = EXPR_SUB;
      else
        break;
      
      expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, mul() });
    }

    return expr;
  }

  return node;
}

Node* Parser::assign() {
  auto x = add();

  if( consume("=") ) {
    auto y = new Node(NODE_ASSIGN);

    y->expr = x;

    do {
      y->list.emplace_back(add());
    } while( consume("=") );

    x = y;
  }

  return x;
}

Node* Parser::expr() {
  if( consume("var") ) {
    auto node = new Node(NODE_VAR);
    node->token = consumed;

    expect_ident();
    node->name = token->str;

    next();

    if( consume(":") ) {
      node->type = expect_type();
    }

    if( consume("=") ) {
      node->expr = expr();
    }

    if( !node->type && !node->expr ) {
      error(ERR_TYPE, node->token, "cannot infer a type of variable");
    }
    
    return node;
  }

  if( consume("return") ) {
    auto node = new Node(NODE_RETURN);

    node->token = consumed;
    node->expr = expr();

    return node;
  }

  return assign();
}
