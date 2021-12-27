#include "error.h"
#include "debug.h"
#include "Token.h"
#include "Object.h"
#include "Node.h"
#include "Evaluater.h"

ObjectType Evaluater::evaluate(Node* node) {
  if( !node )
    return { };

  if( node->evaluated )
    return node->objtype;

  node->evaluated = true;

  switch( node->kind ) {
    case NODE_VALUE:
    case NODE_ARRAY:
    case NODE_VARIABLE:
    case NODE_REFERENCE:
    case NODE_CALLFUNC:
    case NODE_ARGUMENT:
      return primary(node);

    case NODE_TYPE:
      return type(node);

    case NODE_ASSIGN:
    case NODE_EXPR:
      return expr(node);

    case NODE_IF:
    case NODE_VAR:
    case NODE_RETURN:
    case NODE_SCOPE:
      return stmt(node);

    case NODE_FUNCTION: {
      in_main = node->name == "main";

      for( auto&& arg : node->list ) {
        auto& obj = node->expr->objects.emplace_back();

        obj.type = evaluate(arg->type);
        obj.name = arg->name;
        obj.scope_depth = scope_depth;
      }

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
            std::cout << Global::get_instance()->file_path << ": In function '" << node->name << "'" << std::endl;
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
      break;
    }
  }

  return { };
}