#include "jstring.h"

bool String_equal(struct String const* lhs, struct String const* rhs)
{
  if (lhs->length != rhs->length)
  {
    return false;
  }
  for (int i = 0; i < lhs->length; ++i)
  {
    if (lhs->data[i] != rhs->data[i])
    {
      return false;
    }
  }
  return true;
}
