#ifndef JAMCC_TOKENIZER_H
#define JAMCC_TOKENIZER_H

#include "utility.h"

// the different types of tokens that exist
enum TokenKind
{
  TK_RESERVED,
  TK_NUMBER,    // [0-9]+
  TK_PLUS,      // +
  TK_MINUS,     // -
  TK_ASTERISK,  // *
  TK_BACKSLASH, // /
  TK_PAREN_L,   // (
  TK_PAREN_R,   // )

  TK_END,
};

// a singular representable unit of code
struct Token
{
  enum TokenKind kind;
  //int line;       // what line number the token came from
  //int column;     // what column number the token came from
  int value;        // for TK_NUMBER
  char const *str;
};

struct Token Token_create(enum TokenKind kind, char const *str);

struct Token Token_create_number(int value, char const *str);

struct Token Token_create_end();

// a stream of tokens
struct TokenStream
{
  struct TokenStreamNode
  {
    enum
    {
      TOKENS_PER_NODE = 16
    } NodeConsts;
    struct TokenStreamNode *next;
    int first, count;
    struct Token tokens[TOKENS_PER_NODE];
  } *head, *tail;
};

bool TokenStream_empty(struct TokenStream* stream);

void TokenStream_push(struct TokenStream* stream, struct Token tok);

void TokenStream_pop(struct TokenStream *stream);

struct Token TokenStream_next(struct TokenStream *stream);

bool TokenStream_end(struct TokenStream* stream);

int TokenStream_expect_number(struct TokenStream *stream);

void TokenStream_expect(struct TokenStream *stream, enum TokenKind kind);

bool TokenStream_consume(struct TokenStream *stream, enum TokenKind kind);

// tokenize the input string and create token stream
struct TokenStream tokenize(char const *p);

#endif // JAMCC_TOKENIZER_H
