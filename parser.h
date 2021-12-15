#ifndef JAMCC_PARSER_H
#define JAMCC_PARSER_H

#include "jamcc.h"
#include "tokenizer.h"

struct Parser;

typedef struct Expression* (*PrefixParseletFn)(struct Parser* parser, struct Token tok);

struct PrefixParselet
{
  PrefixParseletFn fn;
};

typedef struct Expression* (*InfixParseletFn)(struct Parser* parser, struct Expression* left, struct Token tok);

struct InfixParselet
{
  InfixParseletFn fn;
  int precedence;
  enum OperatorAssociativity associativity;
};

/* prefix operators */

struct Expression* PrefixOperatorParseletFn(struct Parser* parser, struct Token tok);
struct Expression* GroupParseletFn(struct Parser* parser, struct Token tok);
struct Expression* LiteralParseletFn(struct Parser* parser, struct Token tok);

/* infix operators */

struct Expression* BinaryOperatorParseletFn(struct Parser* parser, struct Expression* left, struct Token tok);
struct Expression* TernaryOperatorParseletFn(struct Parser* parser, struct Expression* left, struct Token tok);

struct Parser
{
  struct TokenStream* stream;
  // a parselet function for each token kind
  struct PrefixParselet prefix_parselets[TK_COUNT_];
  struct InfixParselet infix_parselets[TK_COUNT_];
};

int Parser_get_precedence(struct Parser* parser);
struct Expression* Parser_parse_expression(struct Parser* parser);
struct Expression* Parser_parse_expression_precedence(struct Parser* parser, int precendence);
void Parser_register_prefix(struct Parser* parser, enum TokenKind kind, PrefixParseletFn parselet_fn);
void Parser_register_infix(struct Parser* parser, enum TokenKind kind, InfixParseletFn parselet_fn, int precedence, enum OperatorAssociativity associativity);
void Parser_prefix_operator(struct Parser* parser, enum TokenKind kind);
void Parser_binary_operator(struct Parser* parser, enum TokenKind kind, int precedence, enum OperatorAssociativity associativity);

struct Statement Parser_parse_statement(struct Parser* parser);

struct Program* Parser_parse_program(struct Parser* parser);

#endif // JAMCC_PARSER_H
