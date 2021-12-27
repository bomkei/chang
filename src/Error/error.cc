#include <cstdarg>
#include "error.h"
#include "values.h"
#include "Driver.h"

void error(ErrorKind kind, Token* token, char const* fmt, ...) {
  static char buf[0x1000];

  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);

  auto globl = Global::get_instance();
  auto const& src = globl->source;

  std::size_t line = 1;
  std::size_t column = 0;
  std::size_t line_begin = 0, line_end = src.length();

  if( kind != ERR_NOTE && kind != ERR_WARN ) {
    globl->is_error_occurred = true;
  }

  for( std::size_t i = 0; i < token->pos; i += 1 ) {
    if( src[i] == '\n' ) {
      line_begin = i + 1;
      line += 1;
    }
  }

  for( std::size_t i = token->pos; i < src.length(); i += 1 ) {
    if( src[i] == '\n' ) {
      line_end = i;
      break;
    }
  }

  printf(COL_WHITE "%s:%lu:%lu: %s: " COL_DEFAULT "%s\n",
    Global::get_instance()->file_path.c_str(), line, column,
      kind == ERR_WARN ? COL_MAGENTA "warning" : (kind == ERR_NOTE ? COL_SKYBLUE "note" : COL_RED "error"), buf);

  printf("%6lu | %s\n",
    line, src.substr(line_begin, line_end - line_begin).c_str());

  printf("       | %s^%s\n\n",
    std::string(token->pos - line_begin, ' ').c_str(),
    std::string(token->str.length() ? token->str.length() - 1 : 0, '~').c_str());
}