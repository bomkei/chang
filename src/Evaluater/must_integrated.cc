#include "token.h"
#include "object.h"
#include "node.h"
#include "error.h"
#include "debug.h"
#include "evaluater.h"

ObjectType Evaluater::must_integrated(Node* scope, std::vector<Node*> const& types) {
  assert(scope->kind == NODE_SCOPE);

  if( types.empty() ) {
    return { };
  }

  auto const first = evaluate(types[0]);
  auto const&& firststr = first.to_string();

  if( types.size() <= 1 ) {
    return first;
  }

  for( auto it = types.begin() + 1; it != types.end(); it++ ) {
    auto&& eval = evaluate(*it);

    if( !first.equals(eval) ) {
      error(ERR_TYPE, scope->token, "all values which can be return value of scope are must be integrated by one type.");
      error(ERR_NOTE, types[0]->token, "return type was inferred as '%s' here", firststr.c_str());
      error(ERR_TYPE, (*it)->token, "expected '%s', but found '%s'", firststr.c_str(), eval.to_string().c_str());
      exit(1);
    }
  }

  return first;
}