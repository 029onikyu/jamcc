#ifndef JAMCC_UTILITY_H
#define JAMCC_UTILITY_H

typedef enum bool_s
{
  false,
  true
} bool;

// logs a fatal formatted error
void fatal_error(char const *fmt, ...);

#endif // JAMCC_UTILITY_H
