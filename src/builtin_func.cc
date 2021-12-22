#include "chang.h"

#define FUNC  [] (Args args) -> Object

using Args = std::vector<Object>&;

static std::vector<BuiltinFunc>* list = nullptr;

std::vector<BuiltinFunc> const& BuiltinFunc::get_list() {
  if( !list ) {
    list = new std::vector<BuiltinFunc>({
      BuiltinFunc(
        "print", true, { }, OBJ_NONE, FUNC {
        for( auto&& i : args ) {
          std::cout << i;
        }

        std::cout << std::endl;
        return { };
      }),
    });
  }

  return *list;
}