#ifndef LEXER_H
#define LEXER_H

#include "ferrum.h"

#include <stddef.h>

typedef struct Token Token;
struct Token {
  u64 line_num;
  char *value;
  Token *next;
};

Token *lexer(const char *buffer);
void print_tokens(Token *head);
void free_tokens(Token *head);

#endif