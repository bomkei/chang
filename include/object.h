#pragma once

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

  bool equals(ObjectType const&) const;
  std::string to_string() const;
};

struct Object {
  ObjectType type;
  std::string_view name;

  union {
    long v_int = 0;
    char16_t v_char;
    double v_float;
    bool v_bool;
    Object* address;
  };

  std::u16string v_str;
  std::vector<Object> list;

  std::string to_string() const;
};
