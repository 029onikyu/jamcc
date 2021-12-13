#include "utility.h"
#include "tokenizer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static int depth;

static void push(void)
{
  printf("  push rax\n");
  ++depth;
}

static void pop(char const *arg)
{
  printf("  pop %s\n", arg);
  --depth;
}

void generate(struct Expression *node)
{
  if (node->kind == EK_LITERAL)
  {
    printf("  mov rax, %d\n", node->literal.integral_value);
    return;
  }
  if (node->kind == EK_BINARY)
  {
    generate(node->binary.right);
    push();
    generate(node->binary.left);
    pop("rdi");

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
      break;
      case TK_EQUAL:
      case TK_NOT_EQUAL:
      case TK_GT:
      case TK_GTE:
      case TK_LT:
      case TK_LTE:
        printf("  cmp rax, rdi\n");
        if (node->binary.op == TK_EQUAL)
          printf("  sete al\n");
        else if (node->binary.op == TK_NOT_EQUAL)
          printf("  setne al\n");
        else if (node->binary.op == TK_GT)
          printf("  setg al\n");
        else if (node->binary.op == TK_GTE)
          printf("  setge al\n");
        else if (node->binary.op == TK_LT)
          printf("  setl al\n");
        else if (node->binary.op == TK_LTE)
          printf("  setle al\n");
        printf("  movzb rax, al\n");
    }
    return;
  }
  if (node->kind == EK_PREFIX)
  {
    generate(node->prefix.expr);

    switch (node->prefix.op)
    {
      break; case TK_PLUS:
      break; case TK_MINUS:
        printf("  neg rax\n");
      break; case TK_EXCLAMATION:
        printf("  test rax, rax\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
    }
    return;
  }

  fatal_error("Cannot generate code for expression kind id %d", node->kind);
}

void generate_statement(struct Statement *statement)
{
  if (statement->kind == SK_EXPRESSION_STATEMENT)
  {
    generate(statement->expression_statement.expression);
    return;
  }

  fatal_error("Invalid statement kind id %d", statement->kind);
}

// @oni TODO: better data structure
#define MAX_STATEMENTS 256
struct Statement statements[MAX_STATEMENTS];
int statement_count = 0;

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

  while (!TokenStream_end(parser.stream))
  {
    statements[statement_count] = Parser_parse_statement(&parser);
    ++statement_count;
  }

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  for (int i = 0; i < statement_count; ++i)
  {
    generate_statement(&statements[i]);
  }

  printf("  ret\n");

  return 0;
}
