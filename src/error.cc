#include <cstdarg>
#include "chang.h"

void error(ErrorKind kind, Token* token, char const* fmt, ...) {
  static char buf[0x1000];

  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);

  std::size_t line = 0;
  std::size_t column = 0;
  auto globl = Global::get_instance();

  printf("%s:%lu:%lu " COL_RED "error: " COL_DEFAULT "%s\n", globl->file_path.c_str(), line, column, buf);
}