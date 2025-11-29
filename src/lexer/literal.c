#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *allocate_sub_string(const char *buffer, u64 start, u64 end) {
  u64 word_size = end - start;
  char *out = malloc(word_size + 1);
  if (!out) {
    fprintf(stderr, "Memory allocation failed.\n");
    return NULL;
  }
  memcpy(out, buffer + start, word_size);
  out[word_size] = '\0';
  return out;
}

char *get_word(const char *buffer, u64 *index, uint *char_num) {
  u64 start = *index;
  while (isalnum(buffer[*index]) || buffer[*index] == '_') {
    (*index)++;
    (*char_num)++;
  }
  return allocate_sub_string(buffer, start, *index);
}

Token_Type get_word_type(const char *word) {
  Keyword keyword[] = {{"void", VOID}, // type
      {"NULL", NULL_C},                // type
      {"null", NULL_S},                // type
      {"bool", BOOL},                  // type
      {"true", TRUE},                  // type
      {"false", FALSE},                // type
      {"char", CHAR},                  // type
      {"short", SHORT},                // type
      {"int", INT},                    // type
      {"long", LONG},                  // type
      {"float", FLOAT},                // type
      {"double", DOUBLE},              // type
      {"signed", SIGNED},              // type
      {"unsigned", UNSIGNED},          // type
      {"object", OBJECT},              // type
      {"func", FUNC},                  // func
      {"pass", PASS},                  // func
      {"return", RETURN},              // func
      {"if", IF},                      // controlflow
      {"elif", ELIF},                  // controlflow
      {"else", ELSE},                  // controlflow
      {"and", AND},                    // Bit logic
      {"or", OR},                      // Bit logic
      {"not", NOT}};                   // LOGIC

  size_t n_keywords = sizeof(keyword) / sizeof(keyword[0]);
  for (size_t i = 0; i < n_keywords; ++i) {
    if (strcmp(word, keyword[i].name) == 0)
      return keyword[i].type;
  }
  return IDENTIFIER;
}

char *get_digit(const char *buffer, u64 *index, uint *char_num, u64 line_num, uint *is_float) {
  u64 start = *index;
  *is_float = 0;
  while (isdigit(buffer[*index]) || buffer[*index] == '.') {
    if (buffer[*index] == '.') {
      if (!isdigit(buffer[*index + 1])) {
        fprintf(stderr, "Incomplete Float literal at line %lld char %d.\n", line_num, *char_num);
        return NULL;
      }
      (*is_float)++;
    }
    if (*is_float >= 2) {
      fprintf(stderr, "Invalid symbol `.' at line %lld char %d\n", line_num, *char_num);
      return NULL;
    }
    (*index)++;
    (*char_num)++;
  }
  return allocate_sub_string(buffer, start, *index);
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
    return NULL;
  }
  char *out = allocate_sub_string(buffer, start, *index);
  (*index)++;
  return out;
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
    return NULL;
  }
  char *out = allocate_sub_string(buffer, start, *index);
  (*index)++;
  return out;
}