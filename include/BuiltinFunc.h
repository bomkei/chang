#pragma once

#include <vector>
#include "Object.h"

struct BuiltinFunc {
  using FuncPointer = Object(*)(std::vector<Object>&);

  char const* name;
  bool arg_free;
  std::vector<ObjectType> arg_types;
  ObjectType ret_type;
  FuncPointer func;

  static std::vector<BuiltinFunc> const& get_list();

private:
  BuiltinFunc(char const*, bool, std::vector<ObjectType>, ObjectType, FuncPointer);
};
