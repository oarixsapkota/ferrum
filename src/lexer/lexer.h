#ifndef LEXER_H
#define LEXER_H

#include "../inc/ferrum.h"

#include <stddef.h>

typedef enum {
  UNKNOWN = 0,

  INVALID,

  SOT,
  EOT,

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

  ADD,
  SUB,
  MUL,
  DEV,
  MOD,
  NOT,

  INCRIMENT,
  DECRIMENT,

  ASSIGN,
  ADD_EQ,
  SUB_EQ,
  MUL_EQ,
  DEV_EQ,
  MOD_EQ,
  NOT_EQ,

  GREATER,
  SMALLER,
  GREATER_EQ,
  SMALLER_EQ,
  EQUAL,

  AND,
  OR,

  POINTER,
  ADDRESS_OF,
  ARROW,
  DOT,

  O_PREN,
  C_PREN,
  O_SCOPE,
  C_SCOPE,
  O_BBRACK,
  C_BBRACK,
  SEMI_COLN,
  COLN,
  EXPECT,
  COMMA,

  DIRECTIVE

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

char *get_word(const char *buffer, u64 *index, uint *char_num);
Token_Type get_word_type(const char *word);
char *get_digit(const char *buffer, u64 *index, uint *char_num, u64 line_num, uint *is_float);
char *get_char_lit(const char *buffer, u64 *index, uint *char_num, u64 line_num);
char *get_str_lit(const char *buffer, u64 *index, uint *char_num, u64 line_num);

#endif