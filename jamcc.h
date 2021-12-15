#ifndef JAMCC_JAMCC_H
#define JAMCC_JAMCC_H

#include "jstring.h"

// the different types of tokens that exist
enum TokenKind
{
  TK_RESERVED,
  TK_NUMBER,       // [0-9]+
  TK_IDENTIFIER,   // [a-zA-Z][a-zA-Z0-9]*
  TK_PLUS,         // +
  TK_MINUS,        // -
  TK_ASTERISK,     // *
  TK_BACKSLASH,    // /
  TK_PAREN_L,      // (
  TK_PAREN_R,      // )
  TK_EQUAL,        // ==
  TK_NOT_EQUAL,    // !=
  TK_GT,           // >
  TK_GTE,          // >=
  TK_LT,           // <
  TK_LTE,          // <=
  TK_EXCLAMATION,  // !
  TK_EQUALS,       // =
  TK_SEMICOLON,    // ;
  TK_QUESTION,     // ?
  TK_COLON,        // :
  TK_END,

  TK_COUNT_,
};

enum ExpressionKind
{
  EK_PREFIX,
  EK_BINARY,
  EK_LITERAL,
  EK_VARIABLE,
  EK_CONDITIONAL,

  EK_COUNT_
};

struct Variable
{
  struct String name;
  int size;
  int alignment;
  int offset;
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
    struct Variable* variable;
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
    PROGRAM_MAX_STATEMENTS = 256,
    PROGRAM_MAX_VARIABLES = 256
  };
  struct Statement statements[PROGRAM_MAX_STATEMENTS];
  int statement_count;
  struct Variable variables[PROGRAM_MAX_VARIABLES];
  int variable_count;

  int stack_size;
};

#endif
