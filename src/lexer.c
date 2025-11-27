#include "inc/lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *get_word(const char *buffer, u64 *index, uint *char_num) {
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

static char *get_digit(const char *buffer, u64 *index, uint line_num, uint *char_num) {
  u64 start = *index;
  uint is_float = 0;
  while (isdigit(buffer[*index]) || buffer[*index] == '.') {
    if (buffer[*index] == '.') {
      is_float++;
    }
    if (is_float >= 2) {
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

void push_token(Token **head, Token **tail, u64 line_num, char *value) {
  Token *new_token = malloc(sizeof(Token));
  if (!new_token) {
    perror("malloc failed");
    exit(-1);
  }
  new_token->line_num = line_num;
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
    } else if (isspace(buffer[index])) {
      index++;
      char_num++;
    } else if (isdigit(buffer[index]) || buffer[index] == '.') {
      char *digit = get_digit(buffer, &index, line_num, &char_num);
      push_token(&head, &tail, line_num, digit);
    } else if (isalpha(buffer[index]) || buffer[index] == '_') {
      char *word = get_word(buffer, &index, &char_num);
      push_token(&head, &tail, line_num, word);
    } else if (buffer[index] == '/' && buffer[index + 1] == '/') {
      while (buffer[index] != '\n') {
        index++;
      }
    } else if (buffer[index] == '@') {
      push_token(&head, &tail, line_num, NULL);
      index++;
      char_num++;
    } else {
      printf("Invalid symbol `%c' at line %lld char %d\n", buffer[index], line_num, char_num);
      char_num++;
      index++;
    }
  }
  return head;
}

void print_tokens(Token *token) {
  while (token) {
    Token *next = token->next;
    printf("%s\n", token->value);
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