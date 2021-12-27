#include "chang.h"

static Global* _instance;

Global::Global() {
  _instance = this;
}

Global* Global::get_instance() {
  return _instance;
}