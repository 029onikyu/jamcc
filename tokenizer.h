#ifndef JAMCC_TOKENIZER_H
#define JAMCC_TOKENIZER_H

#include "utility.h"
#include "jstring.h"

// the different types of tokens that exist
enum TokenKind
{
  TK_RESERVED,
  TK_NUMBER,       // [0-9]+
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
  TK_SEMICOLON,    // ;
  TK_QUESTION,     // ?
  TK_COLON,        // :
  TK_END,

  TK_COUNT_,
};

// a singular representable unit of code
struct Token
{
  enum TokenKind kind;
  //int line;       // what line number the token came from
  //int column;     // what column number the token came from
  int value;        // for TK_NUMBER
  struct String str;
};

struct Token Token_create(enum TokenKind kind, struct String str);

struct Token Token_create_number(int value, struct String str);

struct Token Token_create_end();

// a stream of tokens
struct TokenStream
{
  struct TokenStreamNode
  {
    enum NodeConsts
    {
      TOKENS_PER_NODE = 16
    };
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

struct Token TokenStream_expect(struct TokenStream *stream, enum TokenKind kind);

struct Token TokenStream_consume(struct TokenStream *stream);

bool TokenStream_discard(struct TokenStream *stream, enum TokenKind kind);

// tokenize the input string and create token stream
struct TokenStream tokenize(char const *p);

#endif // JAMCC_TOKENIZER_H
