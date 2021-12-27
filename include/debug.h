#pragma once

#include <cassert>
#include <cstdio>

#define  __DEBUG__  1

#define  PLEASE_REPORT  "please report to developer with your script file and this code: "

#if __DEBUG__
  #define alert fprintf(stderr,"\t#alert at %s:%d\n",__FILE__,__LINE__)
  #define debug(...) __VA_ARGS__
  #define dbgprint(...) {fprintf(stderr,__VA_ARGS__);fflush(stderr);}
  #define loop(n,...)  for(int i=0;i<n;i++){__VA_ARGS__}
  #define print_line(...) {fprintf(stderr,"\n------------------\n%s\tline %6d| %s\n\n",__FILE__,__LINE__,#__VA_ARGS__); __VA_ARGS__ }
#else
  #define alert 0
  #define debug(...) 0;
  #define dbgprint(...) 0
  #define loop(...) 0
  #define print_line(...)  __VA_ARGS__
#endif
