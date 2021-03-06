#pragma once

#include <vector>
#include <string>

enum ObjectKind {
  OBJ_INT,
  OBJ_CHAR,
  OBJ_FLOAT,
  OBJ_BOOL,
  OBJ_STRING,
  OBJ_TUPLE,
  OBJ_NONE
};

struct ObjectType {
  ObjectKind kind;
  std::vector<ObjectType> elems;
  std::size_t arr_depth = 0;
  bool reference = false;
  
  ObjectType(ObjectKind kind = OBJ_NONE)
    : kind(kind) {
  }

  ObjectType(ObjectKind kind, std::size_t arr_depth)
    : kind(kind), arr_depth(arr_depth) {
  }

  bool equals(ObjectType const&) const;
  std::string to_string() const;
};

struct Object {
  ObjectType type;
  std::string_view name;
  std::size_t scope_depth = 0;

  union {
    long v_int = 0;
    char16_t v_char;
    double v_float;
    bool v_bool;
    Object* address;
  };

  std::u16string v_str;
  std::vector<Object> list;

  Object() { }
  Object(Object&&);
  Object(Object const&) = default;

  Object& operator = (Object&&);
  Object& operator = (Object const&) = default;

  bool equals(Object const&) const;
  std::string to_string() const;
};
