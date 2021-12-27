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

  if( in_main && 

  auto func_type = evaluate(node->type);
  auto eval = evaluate(node->expr);

  auto list = get_return_values(node->expr);
  auto err = false;

  if( list.empty() && !func_type.equals(OBJ_NONE) ) {
    error(ERR_TYPE, node->token, "return type is not none, but function will return nothing.");
  }

  for( auto&& i : list ) {
    auto&& e = evaluate(i);

    if( !func_type.equals(e) ) {
      if( !err ) {
        errortext("In function", Utils::str(node->name));
        err = true;
      }

      error(ERR_TYPE, i->token, "expected '%s', but found '%s'", func_type.to_string().c_str(), e.to_string().c_str());
    }
  }

  if( err ) {
    error(
      ERR_NOTE, node->type ? node->type->token : node->token,
      "specified with '%s' here", node->type ? Utils::str(node->type->name) : "int"
    );
  }

  in_main = false;
  return ret;
}