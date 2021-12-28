#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"
#include "Utils.h"

ObjectType Evaluater::func(Node* node) {
  auto& ret = node->objtype;

  in_main = node->name == "main";

  for( auto&& arg : node->list ) {
    auto& obj = node->expr->objects.emplace_back();

    obj.type = evaluate(arg->type);
    obj.name = arg->name;
    obj.scope_depth = scope_depth;
  }

  if( in_main ) {
    if( !node->expr->objects.empty() &&
      !(node->expr->objects.size() == 1 && node->expr->objects[0].type.equals(({ ObjectType t = OBJ_STRING; t.arr_depth = 1; t; }))) ) {
      error(ERR_MAIN_FUNC, node->token, "arguments of 'main' function are must empty, or string[]");
    }
  }

  auto func_type = evaluate(node->type);
  auto eval = evaluate(node->expr);

  auto list = get_return_values(node->expr);
  auto scope_type = must_integrated(node->expr, list);
  auto err = false;

  if( list.empty() && !func_type.equals(OBJ_NONE) ) {
    error(ERR_TYPE, node->token, "return type is not none, but function will return nothing.");
  }

  if( in_main ) {
    if( !scope_type.equals(OBJ_INT) && !scope_type.equals(OBJ_NONE) ) {
      error(ERR_TYPE, node->expr->token, "code of main is must return int or nothing");
    }
  }
  else if( !func_type.equals(scope_type) ) {
    error(ERR_TYPE, node->expr->token, "type mismatch");
    error(ERR_NOTE, node->type->token, "specified '%s', but code will return '%s'", func_type.to_string().c_str(), scope_type.to_string().c_str());
  }

  in_main = false;
  return ret;
}