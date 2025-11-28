#ifndef LEXER_H
#define LEXER_H

#include "ferrum.h"

#include <stddef.h>

typedef enum {
  UNKNOWN = 0,

  IDENTIFIER,
  LABEL,

  FLOAT_LIT,
  INT_LIT,
  CHAR_LIT,
  STRING_LIT,
  FILE_LIT,

  VOID,
  NULL_C,
  NULL_S,
  BOOL,
  TRUE,
  FALSE,
  CHAR,
  SHORT,
  INT,
  LONG,
  FLOAT,
  DOUBLE,
  SIGNED,
  UNSIGNED,
  STRING,
  OBJECT,

  FUNC,
  RETURN,
  PASS,
  EXIT,

  IF,
  ELIF,
  ELSE,
  WHILE,
  GUARD,
  GOTO,
  GOIF,

  O_PREN,
  C_PREN,
  O_SCOPE,
  C_SCOPE,
  SEMI_COLN,
  COLN,
  EXPECT
} Token_Type;

typedef struct Token Token;
struct Token {
  Token_Type type;
  char *value;
  Token *next;
};

typedef struct Keyword Keyword;
struct Keyword {
  const char *name;
  Token_Type type;
};

Token *lexer(const char *buffer);
void print_tokens(Token *head);
void free_tokens(Token *head);

#endif