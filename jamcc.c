#include "codegen.h"
#include "utility.h"
#include "tokenizer.h"
#include "parser.h"

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

  struct Parser parser = {};
  parser.stream = &stream;

  Parser_prefix_operator(&parser, TK_PLUS);
  Parser_prefix_operator(&parser, TK_MINUS);
  Parser_prefix_operator(&parser, TK_EXCLAMATION);
  Parser_binary_operator(&parser, TK_PLUS,      OP_SUM,        OA_LEFT_ASSOCIATIVE); // addition
  Parser_binary_operator(&parser, TK_MINUS,     OP_SUM,        OA_LEFT_ASSOCIATIVE); // subtraction
  Parser_binary_operator(&parser, TK_ASTERISK,  OP_PRODUCT,    OA_LEFT_ASSOCIATIVE); // multiplication
  Parser_binary_operator(&parser, TK_BACKSLASH, OP_PRODUCT,    OA_LEFT_ASSOCIATIVE); // division
  Parser_binary_operator(&parser, TK_EQUAL,     OP_EQUALITY,   OA_LEFT_ASSOCIATIVE);
  Parser_binary_operator(&parser, TK_NOT_EQUAL, OP_EQUALITY,   OA_LEFT_ASSOCIATIVE);
  Parser_binary_operator(&parser, TK_GT,        OP_RELATIONAL, OA_LEFT_ASSOCIATIVE);
  Parser_binary_operator(&parser, TK_GTE,       OP_RELATIONAL, OA_LEFT_ASSOCIATIVE);
  Parser_binary_operator(&parser, TK_LT,        OP_RELATIONAL, OA_LEFT_ASSOCIATIVE);
  Parser_binary_operator(&parser, TK_LTE,       OP_RELATIONAL, OA_LEFT_ASSOCIATIVE);
  //Parser_binary_operator(&parser, TK_CARET, OP_EXPONENT, OA_RIGHT_ASSOCIATIVE); // exponentiation

  Parser_binary_operator(&parser, TK_EQUALS, OP_ASSIGNMENT, OA_RIGHT_ASSOCIATIVE);

  Parser_register_prefix(&parser, TK_PAREN_L, GroupParseletFn);
  Parser_register_prefix(&parser, TK_NUMBER,  LiteralParseletFn);
  Parser_register_prefix(&parser, TK_IDENTIFIER, VariableParseletFn);

  struct Program* program = Parser_parse_program(&parser);

  generate_program(program);

  return 0;
}
