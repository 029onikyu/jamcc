#include "utility.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void fatal_error(char const *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  // breakpoint
  asm ("int3");
  exit(1);
}
