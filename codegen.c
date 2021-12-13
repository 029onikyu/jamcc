#include "codegen.h"

#include <stdio.h>

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

void generate_expression(struct Expression *node)
{
  if (node->kind == EK_LITERAL)
  {
    printf("  mov rax, %d\n", node->literal.integral_value);
    return;
  }
  if (node->kind == EK_BINARY)
  {
    generate_expression(node->binary.right);
    push();
    generate_expression(node->binary.left);
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
    generate_expression(node->prefix.expr);

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
    generate_expression(statement->expression_statement.expression);
    return;
  }

  fatal_error("Invalid statement kind id %d", statement->kind);
}

void generate_program(struct Program* program)
{
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  for (int i = 0; i < program->statement_count; ++i)
  {
    generate_statement(&program->statements[i]);
  }

  printf("  ret\n");

}
