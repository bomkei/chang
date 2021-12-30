#include <algorithm>
#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

ObjectType Evaluater::compare(Node* node) {
  auto& ret = node->objtype;

  ret = evaluate(node->expr_list[0].item);

  if( !ret.equals(OBJ_INT) && !ret.equals(OBJ_FLOAT) ) {
    error(ERR_TYPE, node->token, "invalid operator");
    exit(1);
  }

  for( auto it = node->expr_list.begin() + 1; it != node->expr_list.end(); it++ ) {
    if( !ret.equals(evaluate(it->item)) ) {
      error(ERR_TYPE, it->token, "type mismatch");
      exit(1);
    }
  }

  return OBJ_BOOL;
}