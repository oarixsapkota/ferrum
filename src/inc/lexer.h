#ifndef LEXER_H
#define LEXER_H

#include "ferrum.h"

#include <stddef.h>

typedef enum {
  UNKNOWN = 0,
  IDENTIFIER = 1,
  
  FLOAT_LIT,
  INT_LIT
} Token_Type;

typedef struct Token Token;
struct Token {
  Token_Type type;
  char *value;
  Token *next;
};

Token *lexer(const char *buffer);
void print_tokens(Token *head);
void free_tokens(Token *head);

#endif