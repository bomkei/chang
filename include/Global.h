#pragma once

#include <string>

struct Node;

struct Global {
  std::string source;
  std::string file_path;
  Node* entry_point = nullptr;
  Node* top_node = nullptr;
  Node* main_zero = nullptr;

  bool is_error_occurred = false;

  static Global* get_instance();

private:
  Global();
  friend class Driver;
};
