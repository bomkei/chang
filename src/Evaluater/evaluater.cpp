/*
  todo: insert color before type name in error message

*/

#include "Evaluater.h"

static Evaluater* _instance;

Evaluater::Evaluater() {
  _instance = this;
}

Evaluater* Evaluater::get_instance() {
  return _instance;
}