#include "tokenizer.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Token Token_create(enum TokenKind kind, struct String str)
{
  struct Token tok = {};
  tok.kind = kind;
  tok.str = str;
  return tok;
}

struct Token Token_create_number(int value, struct String str)
{
  struct Token tok = {};
  tok.kind = TK_NUMBER;
  tok.value = value;
  tok.str = str;
  return tok;
}

struct Token Token_create_end()
{
  struct Token tok = {};
  tok.kind = TK_END;
  return tok;
}

bool TokenStream_empty(struct TokenStream* stream)
{
  return (stream->head == NULL);
}

void TokenStream_push(struct TokenStream* stream, struct Token tok)
{
  // case for creating the stream
  if (!stream->head)
  {
    stream->head = (struct TokenStreamNode *)calloc(1, sizeof(struct TokenStreamNode));
    stream->tail = stream->head;
  }

  // tail is full, make a new node
  if (stream->tail->count == TOKENS_PER_NODE)
  {
    stream->tail->next = (struct TokenStreamNode *)calloc(1, sizeof(struct TokenStreamNode));
    stream->tail = stream->tail->next;
  }

  // tail has room, just insert in the array
  stream->tail->tokens[stream->tail->count] = tok;
  ++stream->tail->count;
}

void TokenStream_pop(struct TokenStream *stream)
{
  if (TokenStream_empty(stream))
  {
    return;
  }

  ++stream->head->first;
  --stream->head->count;
  if (stream->head->first > TOKENS_PER_NODE || stream->head->count == 0)
  {
    // remove the last one from the list
    if (stream->tail == stream->head)
      stream->tail = NULL;

    struct TokenStreamNode *to_free = stream->head;
    stream->head = stream->head->next;
    free(to_free);
  }
}

struct Token TokenStream_next(struct TokenStream *stream)
{
  return stream->head->tokens[stream->head->first];
}

bool TokenStream_end(struct TokenStream* stream)
{
  return TokenStream_next(stream).kind == TK_END;
}

struct Token TokenStream_expect(struct TokenStream *stream, enum TokenKind kind)
{
  struct Token tok = TokenStream_next(stream);
  if (tok.kind != kind)
  {
    fatal_error("Expected TokenKind id '%d'", kind);
  }
  TokenStream_pop(stream);
  return tok;
}

struct Token TokenStream_consume(struct TokenStream *stream)
{
  struct Token tok = TokenStream_next(stream);
  TokenStream_pop(stream);
  return tok;
}

bool TokenStream_discard(struct TokenStream *stream, enum TokenKind kind)
{
  struct Token tok = TokenStream_next(stream);
  if (tok.kind != kind)
  {
    return false;
  }
  TokenStream_pop(stream);
  return true;
}

// tokenize the input string and create token stream
struct TokenStream tokenize(char const *p)
{
  struct TokenStream stream;
  memset(&stream, 0, sizeof(struct TokenStream));

  while (*p)
  {
    // skip whitespace
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (*p == '+')
    {
      TokenStream_push(&stream, Token_create(TK_PLUS, (struct String){p++, 1}));
      continue;
    }
    else if (*p == '-')
    {
      TokenStream_push(&stream, Token_create(TK_MINUS, (struct String){p++, 1}));
      continue;
    }
    else if (*p == '*')
    {
      TokenStream_push(&stream, Token_create(TK_ASTERISK, (struct String){p++, 1}));
      continue;
    }
    else if (*p == '/')
    {
      TokenStream_push(&stream, Token_create(TK_BACKSLASH, (struct String){p++, 1}));
      continue;
    }
    else if (*p == '(')
    {
      TokenStream_push(&stream, Token_create(TK_PAREN_L, (struct String){p++, 1}));
      continue;
    }
    else if (*p == ')')
    {
      TokenStream_push(&stream, Token_create(TK_PAREN_R, (struct String){p++, 1}));
      continue;
    }
    else if (*p == '=')
    {
      if (p[1] == '=')
      {
        TokenStream_push(&stream, Token_create(TK_EQUAL, (struct String){p, 2}));
        p += 2;
        continue;
      }
    }
    else if (*p == '!')
    {
      if (p[1] == '=')
      {
        TokenStream_push(&stream, Token_create(TK_NOT_EQUAL, (struct String){p, 2}));
        p += 2;
        continue;
      }
      TokenStream_push(&stream, Token_create(TK_EXCLAMATION, (struct String){p++, 1}));
      continue;
    }
    else if (*p == '>')
    {
      if (p[1] == '=')
      {
        TokenStream_push(&stream, Token_create(TK_GTE, (struct String){p, 2}));
        p += 2;
        continue;
      }
      TokenStream_push(&stream, Token_create(TK_GT, (struct String){p++, 1}));
      continue;
    }
    else if (*p == '<')
    {
      if (p[1] == '=')
      {
        TokenStream_push(&stream, Token_create(TK_LTE, (struct String){p, 2}));
        p += 2;
        continue;
      }
      TokenStream_push(&stream, Token_create(TK_LT, (struct String){p++, 1}));
      continue;
    }

    if (isdigit(*p))
    {
      char const *str = p;
      long value = strtol(p, (char **)&p, 10);
      TokenStream_push(&stream, Token_create_number(value, (struct String){str, p - str}));
      continue;
    }

    fatal_error("Tokenizer can't parse '%c'", *p);
  }

  TokenStream_push(&stream, Token_create_end());
  return stream;
}
