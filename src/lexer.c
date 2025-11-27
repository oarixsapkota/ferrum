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
    } else if (isspace(buffer[index])) {
      index++;
      char_num++;
    } else if (isdigit(buffer[index]) || buffer[index] == '.') {
      uint is_float;
      char *digit = get_digit(buffer, &index, line_num, &char_num, &is_float);
      Token_Type type = (is_float >= 1) ? FLOAT_LIT : INT_LIT;
      push_token(&head, &tail, type, digit);
    } else if (isalpha(buffer[index]) || buffer[index] == '_') {
      char *word = get_word(buffer, &index, &char_num);
      Token_Type type = get_word_type(word);
      if (type != IDENTIFIER) {
        free(word);
        word = NULL;
        push_token(&head, &tail, type, word);
      } else if (type == IDENTIFIER) {
        push_token(&head, &tail, IDENTIFIER, word);
      }
    } else if (buffer[index] == '/' && buffer[index + 1] == '/') {
      while (buffer[index] != '\n') {
        index++;
      }
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