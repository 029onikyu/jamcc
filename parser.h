#ifndef JAMCC_PARSER_H
#define JAMCC_PARSER_H

#include "tokenizer.h"

struct Parser;

enum ExpressionKind
{
  EK_PREFIX,
  EK_BINARY,
  EK_LITERAL,
  EK_VARIABLE,
  EK_CONDITIONAL,

  EK_COUNT_
};

struct Expression
{
  enum ExpressionKind kind;
  union 
  {
    struct
    {
      enum TokenKind op;
      struct Expression* expr;
    } prefix;
    struct 
    {
      enum TokenKind op;
      struct Expression* left;
      struct Expression* right;
    } binary;
    struct
    {
      struct Expression* condition;
      struct Expression* then_branch;
      struct Expression* else_branch;
    } conditional;
    struct
    {
      int integral_value;
    } literal;
  };
};

enum OperatorPrecedence
{
  OP_NONE,
  OP_ASSIGNMENT,
  OP_EQUALITY,
  OP_RELATIONAL,
  OP_SUM,
  OP_PRODUCT,
  OP_PREFIX,
  OP_POSTFIX,
  OP_CALL,

  OP_COUNT_
};

enum OperatorAssociativity
{
  OA_LEFT_ASSOCIATIVE  = 0,
  OA_RIGHT_ASSOCIATIVE = 1,

  OA_COUNT_,
};

enum StatementKind
{
  SK_EXPRESSION_STATEMENT,

  SK_COUNT_
};

struct Statement
{
  enum StatementKind kind;
  union
  {
    struct {
      struct Expression* expression;
    } expression_statement;
  };
};

struct Program
{
  // @oni TODO: better data structure
  enum ProgramConsts
  {
    PROGRAM_MAX_STATEMENTS = 256
  };
  struct Statement statements[PROGRAM_MAX_STATEMENTS];
  int statement_count;
};

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

struct Program Parser_parse_program(struct Parser* parser);

#endif // JAMCC_PARSER_H
