#include "error.h"
#include "debug.h"
#include "Utils.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Parser.h"
#include "Global.h"

Node* Parser::primary() {
  if( consume("(") ) {
    auto e = expr();
    expect(")");
    return e;
  }

  if( consume("[") ) {
    auto x = new Node(NODE_ARRAY);
    x->token = consumed;

    if( consume("]") ) {
      return x;
    }

    do {
      x->list.emplace_back(expr());
    } while( consume(",") );

    expect("]");
    return x;
  }

  if( consume("{") ) {
    auto node = new Node(NODE_SCOPE);

    node->token = consumed;

    if( consume("}") ) {
      return node;
    }

    while( check() ) {
      auto item = expr();
      node->list.emplace_back(item);

      if( consume(";") ) {
        if( consume("}") ) {
          node->list.emplace_back(nullptr);
          break;
        }
        else if( token->kind == TOK_EOF ) {
          error(ERR_SYNTAX, node->token, "not closed scope");
        }

        continue;
      }
      else if( token->str != "}" ) {
        switch( item->kind ) {
          case NODE_IF:
            continue;
        }
      }

      expect("}");

      if( is_need_semicolon(item) ) {
        error(ERR_EXPECTED, consumed, "expected ';', because previous statement is can't be return value.");
      }

      break;
    }

    return node;
  }

  if( consume("if") ) {
    auto node = new Node(NODE_IF);
    node->token = consumed;

    node->expr = expr();

    expect("{", false);
    node->if_true = expr();

    if( consume("else") ) {
      if( token->str != "if" ) {
        expect("{", false);
      }

      node->if_else = expr();
    }
    else {
      node->if_else = nullptr;
    }

    return node;
  }

  auto node = new Node(NODE_VALUE);

  node->token = token;

  switch( token->kind ) {
    case TOK_INT:
      node->obj.type = OBJ_INT;
      node->obj.v_int = atoi(token->str.cbegin());
      break;
    
    case TOK_CHAR:
      node->obj.type = OBJ_CHAR;
      node->obj.v_char = *token->str.cbegin();
      break;
    
    case TOK_FLOAT:
      node->obj.type = OBJ_FLOAT;
      node->obj.v_float = atof(token->str.cbegin());
      break;
    
    case TOK_STRING:
      node->obj.type = OBJ_STRING;
      node->obj.v_str = Utils::String::to_utf16(token->str);
      break;
    
    case TOK_IDENT: {
      node->kind = NODE_VARIABLE;
      node->token = token;
      node->name = token->str;

      next();

      if( consume("(") ) {
        node->kind = NODE_CALLFUNC;

        if( consume(")") ) {
          return node;
        }

        do {
          node->list.emplace_back(expr());
        } while( consume(",") );

        expect(")");
      }

      return node;
    }

    case TOK_RESERVED: {
      if( token->str == "true" || token->str == "false" ) {
        node->obj.type = OBJ_BOOL;
        node->obj.v_bool = token->str == "true";
        break;
      }
      else if( token->str == "none" ) {
        break;
      }

      goto unk;
    }
  }

  next();
  return node;

unk:;
  error(ERR_SYNTAX, token, "syntax error");
  exit(1);
}

Node* Parser::indexref() {
  auto node = primary();

  while( consume("[") ) {
    node = new Node(NODE_INDEX_REF, node, expr(), consumed);
    expect("]");
  }

  return node;
}

Node* Parser::member() {
  auto node = indexref();

  while( consume(".") ) {
    auto x = primary();

    if( x->kind == NODE_CALLFUNC ) {
      x->list.insert(x->list.begin(), node);
      node = x;
    }
    else {
      x = new Node(NODE_MEMBER_ACCESS, node, x, consumed);
      node = x;
    }
  }

  return node;
}

Node* Parser::unary() {
  if( consume("-") ) {
    auto x = new Node(NODE_EXPR);

    (x->expr = new Node(NODE_VALUE))->obj.type.kind = OBJ_INT;
    x->expr_list.emplace_back(Node::ExprPair{ EXPR_SUB, consumed, member() });

    return x;
  }

  if( consume("&") ) {
    auto x = new Node(NODE_REFERENCE);

    x->token = consumed;
    x->expr = member();

    return x;
  }

  if( consume("*") ) {
    auto x = new Node(NODE_DEREFERENCE);

    x->token = consumed;
    x->expr = member();

    return x;
  }

  return member();
}
