#ifndef JAMCC_STRING_H
#define JAMCC_STRING_H

#include "utility.h"

struct String
{
  char const* data;
  int length;
};

bool String_equal(struct String const* lhs, struct String const* rhs);

#endif
