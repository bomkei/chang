#include "error.h"
#include "debug.h"
#include "Utils.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Parser.h"
#include "Global.h"

Node* Parser::mul() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(unary());

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

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::add() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(primary());

  while( check() ) {
    ExprKind kind;

    if( consume("+") )
      kind = EXPR_ADD;
    else if( consume("-") )
      kind = EXPR_SUB;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, primary() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::shift() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(add());

  while( check() ) {
    ExprKind kind;

    if( consume("<<") )
      kind = EXPR_LSHIFT;
    else if( consume("<<") )
      kind = EXPR_RSHIFT;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, add() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::spaceship() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(shift());

  while( check() ) {
    ExprKind kind;

    if( consume("<=>") )
      kind = EXPR_SPACESHIP;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, shift() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::compare() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(spaceship());

  while( check() ) {
    ExprKind kind;

    if( consume("<") )
      kind = EXPR_RBIGGER;
    else if( consume(">") )
      kind = EXPR_LBIGGER;
    else if( consume("<=") )
      kind = EXPR_RBIGGER_OR_EQ;
    else if( consume(">=") )
      kind = EXPR_LBIGGER_OR_EQ;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, spaceship() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::equalty() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(compare());

  while( check() ) {
    ExprKind kind;

    if( consume("==") )
      kind = EXPR_EQUAL;
    else if( consume("!=") )
      kind = EXPR_NOT_EQUAL;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, compare() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::bit_and() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(equalty());

  while( check() ) {
    ExprKind kind;

    if( consume("&") )
      kind = EXPR_BIT_AND;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, equalty() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::bit_xor() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(bit_and());

  while( check() ) {
    ExprKind kind;

    if( consume("^") )
      kind = EXPR_BIT_XOR;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, bit_and() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::bit_or() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(bit_xor());

  while( check() ) {
    ExprKind kind;

    if( consume("|") )
      kind = EXPR_BIT_OR;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, bit_xor() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::log_and() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(bit_or());

  while( check() ) {
    ExprKind kind;

    if( consume("&&") )
      kind = EXPR_AND;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, bit_or() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::log_or() {
  auto expr = new Node(NODE_EXPR);

  expr->token = token;
  expr->first_expr(log_and());

  while( check() ) {
    ExprKind kind;

    if( consume("||") )
      kind = EXPR_OR;
    else
      break;

    expr->expr_list.emplace_back(Node::ExprPair{ kind, consumed, log_and() });
  }

  return expr->is_single() ? expr->expr_list[0].item : expr;
}

Node* Parser::assign() {
  auto x = log_or();

  if( consume("=") ) {
    auto y = new Node(NODE_ASSIGN);

    y->expr = x;

    do {
      y->list.emplace_back(log_or());
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
