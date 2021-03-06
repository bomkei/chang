#pragma once

#include <vector>

struct Node;
struct Token;

class Parser {
public:
  explicit Parser(Token*);

  Node* parse();

  Node* primary();
  Node* indexref();
  Node* member();
  Node* unary();
  Node* mul();
  Node* add();
  Node* shift();
  Node* spaceship();
  Node* compare();
  Node* bit_and();
  Node* bit_xor();
  Node* bit_or();
  Node* log_and();
  Node* log_or();
  Node* assign();
  Node* expr();
  Node* top();

private:
  void next();
  bool check();
  bool consume(char const*);
  void expect(char const*, bool = true);
  void expect_ident();

  Node* expect_argument();
  Node* expect_type();

  bool is_need_semicolon(Node* node);

  Token* token;
  Token* consumed;

  std::vector<Node*> structs;
};
