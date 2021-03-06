#include "parser.h"

#include <stdlib.h>

static struct Program* program;

static struct Variable* find_variable(struct String name)
{
  for (int i = 0; i < program->variable_count; ++i)
  {
    if (String_equal(&program->variables[i].name, &name))
    {
      return &program->variables[i];
    }
  }
  return NULL;
}

/* prefix operators */

struct Expression* PrefixOperatorParseletFn(struct Parser* parser, struct Token tok)
{
  struct Expression* operand = Parser_parse_expression_precedence(parser, OP_PREFIX);

  struct Expression* new_expr = calloc(1, sizeof(struct Expression));
  new_expr->kind = EK_PREFIX;
  new_expr->prefix.op = tok.kind;
  new_expr->prefix.expr = operand;

  return new_expr;
}

struct Expression* GroupParseletFn(struct Parser* parser, struct Token tok)
{
  struct Expression* expr = Parser_parse_expression(parser);
  TokenStream_expect(parser->stream, TK_PAREN_R);
  return expr;
}

struct Expression* LiteralParseletFn(struct Parser* parser, struct Token tok)
{
  struct Expression* new_expr = calloc(1, sizeof(struct Expression));
  new_expr->kind = EK_LITERAL;
  new_expr->literal.integral_value = tok.value;
  return new_expr;
}

struct Expression* VariableParseletFn(struct Parser* parser, struct Token tok)
{
  struct Expression* new_expr = calloc(1, sizeof(struct Expression));
  new_expr->kind = EK_VARIABLE;
  struct Variable* variable = find_variable(tok.str);
  if (variable)
  {
    new_expr->variable = variable;
  }
  else
  { 
    // create a new variable
    new_expr->variable = &program->variables[program->variable_count];
    ++program->variable_count;
    new_expr->variable->name = tok.str;
    new_expr->variable->size = 8;
    new_expr->variable->alignment = 8;
  }
  return new_expr;
}

/* infix operators */

struct Expression* BinaryOperatorParseletFn(struct Parser* parser, struct Expression* left, struct Token tok)
{
  struct InfixParselet binary_parselet = parser->infix_parselets[tok.kind];
  struct Expression* right = Parser_parse_expression_precedence(
    parser,
    binary_parselet.precedence - (binary_parselet.associativity)
  );

  struct Expression* new_expr = calloc(1, sizeof(struct Expression));
  new_expr->kind = EK_BINARY;
  new_expr->binary.left = left;
  new_expr->binary.op = tok.kind;
  new_expr->binary.right = right;

  return new_expr;
}

struct Expression* TernaryOperatorParseletFn(struct Parser* parser, struct Expression* left, struct Token tok)
{
  struct Expression* then_branch = Parser_parse_expression(parser);
  TokenStream_expect(parser->stream, TK_COLON);
  struct Expression* else_branch = Parser_parse_expression(parser);

  struct Expression* new_expr = calloc(1, sizeof(struct Expression));
  new_expr->kind = EK_CONDITIONAL;
  new_expr->conditional.condition = left;
  new_expr->conditional.then_branch = then_branch;
  new_expr->conditional.else_branch = else_branch;

  return new_expr;
}

int Parser_get_precedence(struct Parser* parser)
{
  struct InfixParselet infix = parser->infix_parselets[TokenStream_next(parser->stream).kind];
  if (infix.fn)
  {
    return infix.precedence;
  }
  return OP_NONE;
}

struct Expression* Parser_parse_expression(struct Parser* parser)
{
  return Parser_parse_expression_precedence(parser, OP_NONE);
}

struct Expression* Parser_parse_expression_precedence(struct Parser* parser, int precendence)
{
  struct Token tok = TokenStream_consume(parser->stream);
  struct PrefixParselet prefix = parser->prefix_parselets[tok.kind];

  if (!prefix.fn)
  {
    fatal_error("Could not parse '%.*s'", tok.str.length, tok.str.data);
  }

  struct Expression* left = prefix.fn(parser, tok);

  while (precendence < Parser_get_precedence(parser))
  {
    tok = TokenStream_consume(parser->stream);

    struct InfixParselet infix = parser->infix_parselets[tok.kind];
    left = infix.fn(parser, left, tok);
  }

  return left;
}

void Parser_register_prefix(struct Parser* parser, enum TokenKind kind, PrefixParseletFn parselet_fn)
{
  parser->prefix_parselets[kind].fn = parselet_fn;
}

void Parser_register_infix(struct Parser* parser, enum TokenKind kind, InfixParseletFn parselet_fn, int precedence, enum OperatorAssociativity associativity)
{
  struct InfixParselet* p = &parser->infix_parselets[kind];
  p->fn = parselet_fn;
  p->precedence = precedence;
  p->associativity = associativity;
}

void Parser_prefix_operator(struct Parser* parser, enum TokenKind kind)
{
  Parser_register_prefix(parser, kind, PrefixOperatorParseletFn);
}

void Parser_binary_operator(struct Parser* parser, enum TokenKind kind, int precedence, enum OperatorAssociativity associativity)
{
  Parser_register_infix(parser, kind, BinaryOperatorParseletFn, precedence, associativity);
}

struct Statement Parser_parse_statement(struct Parser* parser)
{
  struct Expression* expression = Parser_parse_expression(parser);
  TokenStream_expect(parser->stream, TK_SEMICOLON);

  struct Statement statement;
  statement.kind = SK_EXPRESSION_STATEMENT;
  statement.expression_statement.expression = expression;
  return statement;
}

struct Program* Parser_parse_program(struct Parser* parser)
{
  program = calloc(1, sizeof(struct Program));
  program->statement_count = 0;
  program->variable_count = 0;
  while (!TokenStream_end(parser->stream))
  {
    program->statements[program->statement_count] = Parser_parse_statement(parser);
    ++program->statement_count;
  }
  return program;
}
