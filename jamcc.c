#include "utility.h"
#include "tokenizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fatal_error("Incorrect number of arguments");
  }

  struct TokenStream stream = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  printf("  mov rax, %d\n", TokenStream_expect_number(&stream));

  while (!TokenStream_end(&stream))
  {
    if (TokenStream_consume(&stream, TK_PLUS))
    {
      printf("  add rax, %d\n", TokenStream_expect_number(&stream));
      continue;
    }

    TokenStream_expect(&stream, TK_MINUS);
    printf("  sub rax, %d\n", TokenStream_expect_number(&stream));
  }

  printf("  ret\n");
  return 0;
}
