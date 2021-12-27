
/*
  todo: insert color before type name in error message

*/

static Evaluater* _instance;

Evaluater::Evaluater() {
  _instance = this;
}

Evaluater* Evaluater::get_instance() {
  return _instance;
}

ObjectType Evaluater::evaluate(Node* node) {
  if( !node )
    return { };

  if( node->evaluated )
    return node->objtype;

  auto& ret = node->objtype;
  node->evaluated = true;

  switch( node->kind ) {
    case NODE_VALUE:
      ret = node->obj.type;
      break;
    
    case NODE_VARIABLE: {
      auto [scope, index] = find_var(node->name);

      if( scope ) {
        node->var_scope = scope;
        node->var_index = index;

        node->scope_depth = node->get_var().scope_depth;

        if( initialized.contains(&node->get_var()) && !initialized[&node->get_var()] ) {
          error(ERR_UNINITIALIZED, node->token, "cannot use uninitialized variable");
          exit(1);
        }

        alert;
      #if __DEBUG__
        fprintf(stderr,"node->scope_depth = %lu\n",node->scope_depth);
      #endif

        //ret = scope->objects[index].type;
        ret = node->get_var().type;

        break;
      }

      error(ERR_UNDEFINED, node->token, "undefined variable name '%s'", Utils::str(node->name));
      exit(1);
    }

    case NODE_REFERENCE: {
      if( !is_lvalue(node->expr) ) {
        error(ERR_VALUE_TYPE, node->expr->token, "expression is must lvalue");
        exit(1);
      }

      ret = evaluate(node->expr);
      ret.reference = true;

      // node->expr == NODE_VARIABLE
      
      node->scope_depth = node->expr->scope_depth;
      node->obj.type = ret;
      node->obj.address = node->objptr = get_obj_addr(node->expr);

      alert;
    #if __DEBUG__
      fprintf(stderr,"node->scope_depth = %lu\n",node->scope_depth);
    #endif

      break;
    }
    
    case NODE_ARRAY: {
      if( node->is_allowed_empty_array ) {
        if( node->list.empty() ) {
          return node->objtype;
        }
        
        for( auto&& i : node->list ) {
          i->is_allowed_empty_array = true;
          i->objtype = node->objtype;
        }
      }
      else if( node->list.empty() ) {
        error(ERR_TYPE, node->token, "empty array is invalid");
        exit(1);
      }

      auto first = evaluate(node->list[0]);

      for( auto it = node->list.begin() + 1; it != node->list.end(); it++ ) {
        if( !first.equals(evaluate(*it)) ) {
          error(ERR_TYPE, (*it)->token, "type mismatch");
        }
      }

      first.arr_depth += 1;
      return ret = first;
    }

    case NODE_CALLFUNC: {
      auto const& builtin = BuiltinFunc::get_list();

      std::size_t argc;
      bool arg_free;
      std::vector<ObjectType> args;
      std::vector<Node*> find_userdef;
      Node* fn;

      for( auto&& i : node->list ) {
        args.emplace_back(evaluate(i));
      }

      for( auto&& i : builtin ) {
        if( i.name == node->name ) {
          argc = i.arg_types.size();
          arg_free = i.arg_free;
          node->builtin = &i;
          ret = i.ret_type;
          goto check_process;
        }
      }

      find_userdef = find_func(node->name, args);

      // todo: check finely
      // todo: add find_func_same_name()

      if( find_userdef.empty() ) {
        error(ERR_UNDEFINED, node->token, "undefined function");

        for( auto&& scope : scope_list ) {
          for( auto&& i : scope->list ) {
            if( i && i->kind == NODE_FUNCTION && i->name == node->name )
              find_userdef.emplace_back(i);
          }
        }

        if( !find_userdef.empty() ) {
          error(ERR_NOTE, node->token, "but found some function with same name, maybe did you tried call with wrong arguments?");

          for( auto&& i : find_userdef ) {
            error(ERR_NOTE, i->token, "got this");
          }
        }
      }
      else if( find_userdef.size() > 1 ) {
        error(ERR_MANY_CANDIDATES, node->token, "found many candidates with this name");

        for( auto&& i : find_userdef ) {
          error(ERR_NOTE, i->token, "got this");
        }
      }

      fn = find_userdef[0];

      ret = evaluate(fn->expr);
      node->func = fn;

      break;

    check_process:;
      if( !arg_free ) {
        if( node->list.size() < argc ) {
          error(ERR_ARGUMENT, node->token, "too few argument");
        }
        else if( node->list.size() > argc ) {
          error(ERR_ARGUMENT, node->token, "too many argument");
        }
      }

      break;
    }

    case NODE_TYPE: {
      if( node->name == "int" )
        ret = OBJ_INT;
      else if( node->name == "char" )
        ret = OBJ_CHAR;
      else if( node->name == "float" )
        ret = OBJ_FLOAT;
      else if( node->name == "bool" )
        ret = OBJ_BOOL;
      else if( node->name == "string" )
        ret = OBJ_STRING;
      else if( node->name == "none" )
        ret = OBJ_NONE;
      else {
        // todo: find struct

        error(ERR_TYPE, node->token, "unknown type name");
        exit(1);
      }

      for( auto&& i : node->elemcount_list ) {
        if( i && !evaluate(i).equals(OBJ_INT) ) {
          error(ERR_TYPE, i->token, "elements count is must be integer");
        }

        ret.arr_depth++;
      }

      ret.reference = node->is_reference;
      break;
    }

    case NODE_ARGUMENT: {
      ret = evaluate(node->type);
      break;
    }

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

    case NODE_SCOPE: {
      if( node->list.empty() ) {
        break;
      }

      scope_list.push_front(node);

      scope_depth++;
      node->scope_depth = scope_depth;

      auto returned = false;
      Node* ret_nd;

      for( auto&& i : node->list ) {
        if( !i ) {
          continue;
        }

        if( returned ) {
          if( i == Global::get_instance()->main_zero )
            continue;

          error(ERR_RETURN, i->token, "code is invalid after return statement");
          error(ERR_NOTE, ret_nd->token, "returned here");
        }
        else if( i->kind == NODE_RETURN ) {
          returned = true;
          ret_nd = i;
        }

        i->is_allowed_let = true;
        i->is_allowed_return = true;

        evaluate(i);
      }

      if( node != Global::get_instance()->top_node ) {
        auto const& ret_nodes = get_return_values(node);

        ret = must_integrated(node, ret_nodes);

        for( auto&& i : ret_nodes ) {
          auto&& e = evaluate(i);

          alert;
        #if __DEBUG__
          fprintf(stderr,"%lu %lu\n",node->scope_depth,i->scope_depth);
        #endif

          if( e.reference && node->scope_depth <= i->scope_depth ) {
            error(ERR_LIFE_SPAN, i->token, "this is cannot be take out and use from scope");
          }
        }
      }

      scope_depth--;
      scope_list.pop_front();

      break;
    }

    case NODE_VAR: {
      var_stmt_list.push_front(node);

      if( !node->is_allowed_let ) {
        error(ERR_LOCATION, node->token, "cannot declare variable here");
        exit(1);
      }

      auto [scope, index] = find_var(node->name);

      if( scope == *scope_list.begin() ) {
        error(ERR_MULTIPLE_DEFINED, node->token, "multiple defined variable name");
        exit(1);
      }

      auto cur = *scope_list.begin();
      auto& obj = cur->objects.emplace_back();

      obj.name = node->name;
      obj.scope_depth = cur->scope_depth;

      initialized[&obj] = false;

      alert;
    #if __DEBUG__
      fprintf(stderr,"obj.scope_depth = %lu\n",obj.scope_depth);
    #endif

      node->var_scope = cur;
      node->var_index = cur->objects.size() - 1;

      auto specified_type = evaluate(node->type);

      if( specified_type.reference || (node->expr && node->expr->kind == NODE_REFERENCE) ) {
        if( !node->expr ) {
          error(ERR_REFERENCE, node->token, "must have intiializer expression due to variable type was specified as reference");
          exit(1);
        }
        else if( !is_lvalue(node->expr) ) {
          error(ERR_VALUE_TYPE, node->expr->token, "initializer expression of reference is must lvalue");
          exit(1);
        }
        
        if( ++ref_counter[get_obj_addr(node->expr)] >= 2 ) {
          error(ERR_REFERENCE, node->token, "cannot make two reference for same object");
          exit(1);
        }

        node->get_var().type = specified_type;
        node->get_var().type.reference = true;
        node->get_var().address = get_obj_addr(node->expr);
      }

      if( node->type ) {
        //ObjectType expr_type;

        auto chk_type = node->expr != nullptr;
        obj.type = specified_type;

        if( node->expr && !specified_type.equals(evaluate(node->expr)) ) {
          error(ERR_TYPE, node->token, "type mismatch");
          exit(1);
        }

        if( specified_type.arr_depth ) {
          auto flag = false;

          node->is_make_array = true;

          for( auto it = node->type->elemcount_list.begin(); it != node->type->elemcount_list.end(); it++ ) {
            if( *it == nullptr ) {
              if( !flag ) {
                flag = true;
              }
            }
            else if( flag ) {
              error(ERR_TYPE, (*it)->token, "cannot specify elements count of array in this depth, due to not specified previous depth.");
              error(ERR_NOTE, (*it)->token->back->back->back, "due to elements will be empty in this depth, the next count specification is invalid.");
              return { };
            }

            node->objects.emplace_back();
          }

          if( node->type->elemcount_list[0] ) {
            if( node->expr ) {
              node->expr->is_allowed_empty_array = true;
              (node->expr->objtype = specified_type).arr_depth = 1;
              check_array(node->type->elemcount_list.begin(), node->objects.begin(), specified_type.arr_depth, node->expr);
            }

            var_stmt_list.pop_front();
            break;
          }
        }

        if( chk_type && !evaluate(node->expr).equals(specified_type) ) {
          error(ERR_TYPE, node->token, "type mismatch");
        }
      }
      else {
        obj.type = evaluate(node->expr);
      }

      initialized[&obj] = true;
      var_stmt_list.pop_front();

      break;
    }

    case NODE_IF: {
      if( !evaluate(node->expr).equals(OBJ_BOOL) ) {
        error(ERR_TYPE, node->token, "condition is must boolean");
      }

      ret = evaluate(node->if_true);

      if( node->if_else ) {
        if( !ret.equals(evaluate(node->if_else)) ) {
          error(ERR_TYPE, node->token, "type mismatch");
        }
      }

      break;
    }

    case NODE_RETURN: {
      if( !node->is_allowed_return ) {
        error(ERR_LOCATION, node->token, "cannot use return here");
      }

      return evaluate(node->expr);
    }

    case NODE_ASSIGN: {
      if( !is_lvalue(node->expr) ) {
        error(ERR_VALUE_TYPE, node->expr->token, "expression is must lvalue");
      }

      ret = evaluate(node->expr);

      for( auto&& i : node->list ) {
        if( i != *node->list.rbegin() && !is_lvalue(i) ) {
          error(ERR_VALUE_TYPE, i->token, "expression is must lvalue");
        }

        if( !ret.equals(evaluate(i)) ) {
          error(ERR_TYPE, i->token, "type mismatch");
        }
      }

      break;
    }

    case NODE_EXPR: {
      ret = evaluate(node->expr);

      for( auto&& item : node->expr_list ) {
        if( !ret.equals(evaluate(item.item)) ) {
          error(ERR_TYPE, item.token, "type mismatch");
        }
      }

      break;
    }
  }
  
  return ret;
}