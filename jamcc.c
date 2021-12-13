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
      break; case TK_PLUS:
        printf("  add rax, rdi\n");
      break; case TK_MINUS:
        printf("  sub rax, rdi\n");
      break; case TK_ASTERISK:
        printf("  imul rax, rdi\n");
      break; case TK_BACKSLASH:
        printf("  cqo\n");
        printf("  idiv rdi\n");
      break; case TK_EQUAL:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
      break; case TK_NOT_EQUAL:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
      break; case TK_GT:
        printf("  cmp rax, rdi\n");
        printf("  setg al\n");
        printf("  movzb rax, al\n");
      break; case TK_GTE:
        printf("  cmp rax, rdi\n");
        printf("  setge al\n");
        printf("  movzb rax, al\n");
      break; case TK_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
      break; case TK_LTE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
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
      case TK_EXCLAMATION:
        printf("  mov rdx, rax\n");
        printf("  xor rax, rax\n");
        printf("  test rdx, rdx\n");
        printf("  sete al\n");
        break;
    }

    printf("  push rax\n");
    return;
  }
  fatal_error("Cannot generate code for expression kind id %d", node->kind);
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
  Parser_register_prefix(&parser, TK_PAREN_L, GroupParseletFn);
  Parser_register_prefix(&parser, TK_NUMBER,  LiteralParseletFn);

  struct Statement* statement = Parser_parse_statement(&parser);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  generate(statement->expression_statement.expression);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
