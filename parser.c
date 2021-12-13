#include "parser.h"

#include <stdlib.h>

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
    fatal_error("Could not parse '%s'", tok.str);
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
