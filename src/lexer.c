#include "inc/lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_word(const char *buffer, u64 *index, uint *char_num) {
  u64 start = *index;
  while (isalnum(buffer[*index]) || buffer[*index] == '_') {
    (*index)++;
    (*char_num)++;
  }
  u64 word_size = *index - start;
  char *out_buffer = malloc(word_size + 1);
  if (!out_buffer) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  memcpy(out_buffer, buffer + start, word_size);
  out_buffer[word_size] = '\0';
  return out_buffer;
}

Token_Type get_word_type(const char *word) {
  if (!strcmp("if", word)) {
    return IDENTIFIER;
  } else if (!strcmp("elif", word)) {
    return IDENTIFIER;
  } else if (!strcmp("else", word)) {
    return IDENTIFIER;
  } else {
    return IDENTIFIER;
  }
}

char *get_digit(const char *buffer, u64 *index, uint line_num, uint *char_num, uint *is_float) {
  u64 start = *index;
  *is_float = 0;
  while (isdigit(buffer[*index]) || buffer[*index] == '.') {
    if (buffer[*index] == '.') {
      (*is_float)++;
    }
    if (*is_float >= 2) {
      printf("Invalid symbol `.' at line %d char %d\n", line_num, *char_num);
      break;
    }
    (*index)++;
    (*char_num)++;
  }
  u64 word_size = *index - start;
  char *out_buffer = malloc(word_size + 1);
  if (!out_buffer) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  memcpy(out_buffer, buffer + start, word_size);
  out_buffer[word_size] = '\0';
  return out_buffer;
}

char *get_str_lit(const char *buffer, u64 *index, uint *char_num, u64 line_num) {
  (*index)++;
  u64 start = *index;
  while (buffer[*index] != '"' && buffer[*index] != '\0' && buffer[*index] != '\n') {
    (*index)++;
    (*char_num)++;
  }
  if (buffer[*index] == '\0' || buffer[*index] == '\n') {
    fprintf(stderr, "Unterminated String literal at line %lld char %d.\n", line_num, *char_num);
    exit(-1);
  }
  u64 word_size = *index - start;
  char *out_buffer = malloc(word_size + 1);
  if (!out_buffer) {
    perror("malloc failed");
    exit(-1);
  }
  memcpy(out_buffer, buffer + start, word_size);
  out_buffer[word_size] = '\0';
  (*index)++;
  return out_buffer;
}

char *get_char_lit(const char *buffer, u64 *index, uint *char_num, u64 line_num) {
  (*index)++;
  u64 start = *index;
  while (buffer[*index] != '\'' && buffer[*index] != '\0' && buffer[*index] != '\n') {
    (*index)++;
    (*char_num)++;
  }
  if (buffer[*index] == '\0' || buffer[*index] == '\n') {
    fprintf(stderr, "Unterminated character literal at line %lld char %d.\n", line_num, *char_num);
    exit(-1);
  }
  u64 word_size = *index - start;
  if (word_size >= 3) {
    fprintf(stderr, "Character literal too long at line %lld char %d.\n", line_num, *char_num);
    exit(-1);
  }
  char *out_buffer = malloc(word_size + 1);
  if (!out_buffer) {
    perror("malloc failed");
    exit(-1);
  }
  memcpy(out_buffer, buffer + start, word_size);
  out_buffer[word_size] = '\0';
  (*index)++;
  return out_buffer;
}

void push_token(Token **head, Token **tail, Token_Type type, char *value) {
  Token *new_token = malloc(sizeof(Token));
  if (!new_token) {
    perror("malloc failed");
    exit(-1);
  }
  new_token->type = type;
  new_token->value = value;
  new_token->next = NULL;

  if (*tail) {
    (*tail)->next = new_token;
  } else {
    *head = new_token;
  }
  *tail = new_token;
}

Token *lexer(const char *buffer) {
  u64 index = 0;
  u64 line_num = 1;
  uint char_num = 1;
  Token *head = NULL;
  Token *tail = NULL;

  while (buffer[index] != '\0') {
    if (buffer[index] == '\n') {
      line_num++;
      char_num = 1;
      index++;
      continue;
    } else if (isspace(buffer[index])) {
      index++;
      char_num++;
      continue;
    } else if (isdigit(buffer[index]) || buffer[index] == '.') {
      uint is_float;
      char *digit = get_digit(buffer, &index, line_num, &char_num, &is_float);
      Token_Type type = (is_float >= 1) ? FLOAT_LIT : INT_LIT;
      push_token(&head, &tail, type, digit);
      continue;
    } else if (isalpha(buffer[index]) || buffer[index] == '_') {
      char *word = get_word(buffer, &index, &char_num);
      Token_Type type = get_word_type(word);
      if (type != IDENTIFIER) {
        free(word);
        word = NULL;
        push_token(&head, &tail, type, word);
      } else if (type == IDENTIFIER) {
        push_token(&head, &tail, type, word);
      }
      continue;
    } else if (buffer[index] == '/' && buffer[index + 1] == '/') {
      while (buffer[index] != '\n') {
        index++;
      }
      continue;
    } else if (buffer[index] == '\'') {
      char *char_lit = get_char_lit(buffer, &index, &char_num, line_num);
      push_token(&head, &tail, 0, char_lit);
      continue;
    } else if (buffer[index] == '\"') {
      char *str_lit = get_str_lit(buffer, &index, &char_num, line_num);
      push_token(&head, &tail, 0, str_lit);
      continue;
    } else {
      fprintf(stderr, "Invalid symbol `%c' at line %lld char %d\n", buffer[index], line_num, char_num);
      char_num++;
      index++;
      continue;
    }
  }
  return head;
}

void print_tokens(Token *token) {
  while (token) {
    Token *next = token->next;
    switch (token->type) {
      case 0:
        printf("%s\nTOKEN : UNKNOWN\n", token->value);
        break;
      case IDENTIFIER:
        printf("%s\nTOKEN : IDENTIFIER\n", token->value);
        break;
      case INT_LIT:
        printf("%s\nTOKEN : INT_LIT\n", token->value);
        break;
      case FLOAT_LIT:
        printf("%s\nTOKEN : FLOAT_LIT\n", token->value);
        break;
      default:
        break;
    }
    token = next;
  }
}

void free_tokens(Token *token) {
  while (token) {
    Token *next = token->next;
    if (token->value != NULL) {
      free(token->value);
    }
    free(token);
    token = next;
  }
}