#include "utility.h"
#include "tokenizer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void generate(struct Expression* node)
{
  if (node->kind == EK_LITERAL)
  {
    printf("  push %d\n", node->literal.integral_value);
    return;
  } 
  if (node->kind == EK_BINARY)
  {
    generate(node->binary.left);
    generate(node->binary.right);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->binary.op)
    {
      case TK_PLUS:
        printf("  add rax, rdi\n");
        break;
      case TK_MINUS:
        printf("  sub rax, rdi\n");
        break;
      case TK_ASTERISK:
        printf("  imul rax, rdi\n");
        break;
      case TK_BACKSLASH:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    }

    printf("  push rax\n");
    return;
  }
  if (node->kind == EK_PREFIX)
  {
    generate(node->prefix.expr);

    printf("  pop rax\n");

    switch (node->prefix.op)
    {
      case TK_PLUS:
        break;
      case TK_MINUS:
        printf("  neg rax\n");
        break;
    }

    printf("  push rax\n");
    return;
  }
}

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
  Parser_binary_operator(&parser, TK_PLUS,      OP_SUM,     OA_LEFT_ASSOCIATIVE); // addition
  Parser_binary_operator(&parser, TK_MINUS,     OP_SUM,     OA_LEFT_ASSOCIATIVE); // subtraction
  Parser_binary_operator(&parser, TK_ASTERISK,  OP_PRODUCT, OA_LEFT_ASSOCIATIVE); // multiplication
  Parser_binary_operator(&parser, TK_BACKSLASH, OP_PRODUCT, OA_LEFT_ASSOCIATIVE); // division
  Parser_register_prefix(&parser, TK_PAREN_L, GroupParseletFn);
  Parser_register_prefix(&parser, TK_NUMBER,  LiteralParseletFn);

  struct Expression* expr_tree = Parser_parse_expression(&parser);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  generate(expr_tree);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
