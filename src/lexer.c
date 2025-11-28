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
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  memcpy(out_buffer, buffer + start, word_size);
  out_buffer[word_size] = '\0';
  return out_buffer;
}

Token_Type get_word_type(const char *word) {
  if (!strcmp("void", word)) {
    return VOID;
  } else if (!strcmp("NULL", word)) {
    return NULL_C;
  } else if (!strcmp("null", word)) {
    return NULL_S;
  } else if (!strcmp("bool", word)) {
    return BOOL;
  } else if (!strcmp("true", word)) {
    return TRUE;
  } else if (!strcmp("char", word)) {
    return CHAR;
  } else if (!strcmp("short", word)) {
    return SHORT;
  } else if (!strcmp("int", word)) {
    return INT;
  } else if (!strcmp("long", word)) {
    return LONG;
  } else if (!strcmp("float", word)) {
    return FLOAT;
  } else if (!strcmp("double", word)) {
    return DOUBLE;
  } else if (!strcmp("signed", word)) {
    return SIGNED;
  } else if (!strcmp("unsigned", word)) {
    return UNSIGNED;
  } else if (!strcmp("object", word)) {
    return OBJECT;
  } else if (!strcmp("func", word)) {
    return FUNC;
  } else if (!strcmp("pass", word)) {
    return PASS;
  } else if (!strcmp("return", word)) {
    return RETURN;
  } else if (!strcmp("if", word)) {
    return IF;
  } else if (!strcmp("elif", word)) {
    return ELIF;
  } else if (!strcmp("else", word)) {
    return ELSE;
  } else {
    return IDENTIFIER;
  }
}

char *get_digit(const char *buffer, u64 *index, u64 line_num, uint *char_num, uint *is_float) {
  u64 start = *index;
  *is_float = 0;
  while (isdigit(buffer[*index]) || buffer[*index] == '.') {
    if (buffer[*index] == '.') {
      if (!isdigit(buffer[*index + 1])) {
        fprintf(stderr, "Incomplete Float literal at line %lld char %d.\n", line_num, *char_num);
        *is_float = 999;
        break;
      }
      (*is_float)++;
    }
    if (*is_float >= 2) {
      fprintf(stderr, "Invalid symbol `.' at line %lld char %d\n", line_num, *char_num);
      break;
    }
    (*index)++;
    (*char_num)++;
  }
  u64 word_size = *index - start;
  char *out_buffer = malloc(word_size + 1);
  if (!out_buffer) {
    fprintf(stderr, "Memory allocation failed.\n");
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
    exit(EXIT_FAILURE);
  }
  u64 word_size = *index - start;
  char *out_buffer = malloc(word_size + 1);
  if (!out_buffer) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
  }
  u64 word_size = *index - start;
  if (word_size >= 3) {
    fprintf(stderr, "Character literal too long at line %lld char %d.\n", line_num, *char_num);
    exit(EXIT_FAILURE);
  }
  char *out_buffer = malloc(word_size + 1);
  if (!out_buffer) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  memcpy(out_buffer, buffer + start, word_size);
  out_buffer[word_size] = '\0';
  (*index)++;
  return out_buffer;
}

void push_token(Token **head, Token **tail, Token_Type type, char *value) {
  Token *new_token = malloc(sizeof(Token));
  if (!new_token) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
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
    } else if (isdigit(buffer[index]) || (buffer[index] == '.' && isdigit(buffer[index + 1]))) {
      uint is_float;
      char *digit = get_digit(buffer, &index, line_num, &char_num, &is_float);
      Token_Type type = (is_float >= 1) ? FLOAT_LIT : INT_LIT;
      if (is_float >= 999)
        type = UNKNOWN;
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
        if (buffer[index] == ':') {
          push_token(&head, &tail, LABEL, word);
        } else {
          push_token(&head, &tail, type, word);
        }
      }
      continue;
    } else if (buffer[index] == '/' && buffer[index + 1] == '/') {
      while (buffer[index] != '\n')
        index++;
      continue;
    } else if (buffer[index] == '\'') {
      char *char_lit = get_char_lit(buffer, &index, &char_num, line_num);
      push_token(&head, &tail, CHAR_LIT, char_lit);
      continue;
    } else if (buffer[index] == '\"') {
      char *str_lit = get_str_lit(buffer, &index, &char_num, line_num);
      push_token(&head, &tail, STRING_LIT, str_lit);
      continue;
    } else if (buffer[index] == '@') {
      index++;
      if (!(isalpha(buffer[index]) || buffer[index] == '_')) {
        fprintf(stderr, "Invalid `@' at line %lld char %d\n", line_num, char_num);
        index++;
      }
      char *directive = get_word(buffer, &index, &char_num);
      push_token(&head, &tail, 0, directive);
      continue;
    } else {
      if (buffer[index] == '(') {
        push_token(&head, &tail, O_PREN, NULL);
        index++;
        char_num++;
      } else if (buffer[index] == ')') {
        push_token(&head, &tail, C_PREN, NULL);
        index++;
        char_num++;
      } else if (buffer[index] == '{') {
        push_token(&head, &tail, O_SCOPE, NULL);
        index++;
        char_num++;
      } else if (buffer[index] == '}') {
        push_token(&head, &tail, C_SCOPE, NULL);
        index++;
        char_num++;
      } else if (buffer[index] == ':') {
        push_token(&head, &tail, COLN, NULL);
        index++;
        char_num++;
      } else if (buffer[index] == ';') {
        push_token(&head, &tail, SEMI_COLN, NULL);
        index++;
        char_num++;
      } else if (buffer[index] == '?') {
        push_token(&head, &tail, EXPECT, NULL);
        index++;
        char_num++;
      } else {
        while (isspace(buffer[index]))
          index++;
        fprintf(stderr, "Invalid symbol `%c' at line %lld char %d\n", buffer[index], line_num, char_num);
        char_num++;
        index++;
        continue;
      }
    }
  }
  return head;
}

void print_tokens(Token *token) {
  while (token) {
    Token *next = token->next;
    switch (token->type) {
      case 0:
        printf("%s\t:\tTOKEN : UNKNOWN\n", token->value);
        break;
      case IDENTIFIER:
        printf("%s\t:\tTOKEN : IDENTIFIER\n", token->value);
        break;
      case LABEL:
        printf("%s\t:\tTOKEN : LABEL\n", token->value);
        break;
      case INT_LIT:
        printf("%s\t:\tTOKEN : INT_LIT\n", token->value);
        break;
      case FLOAT_LIT:
        printf("%s\t:\tTOKEN : FLOAT_LIT\n", token->value);
        break;
      case CHAR_LIT:
        printf("%s\t:\tTOKEN : CHAR_LIT\n", token->value);
        break;
      case STRING_LIT:
        printf("%s\t:\tTOKEN : STRING_LIT\n", token->value);
        break;
      case VOID:
        printf("void\t:\tTOKEN : VOID\n");
        break;
      case NULL_C:
        printf("NULL\t:\tTOKEN : NULL_C\n");
        break;
      case NULL_S:
        printf("null\t:\tTOKEN : NULL_S\n");
        break;
      case BOOL:
        printf("bool\t:\tTOKEN : BOOL\n");
        break;
      case TRUE:
        printf("true\t:\tTOKEN : TRUE\n");
        break;
      case FALSE:
        printf("false\t:\tTOKEN : FALSE\n");
        break;
      case CHAR:
        printf("char\t:\tTOKEN : CHAR\n");
        break;
      case SHORT:
        printf("short\t:\tTOKEN : SHORT\n");
        break;
      case INT:
        printf("int\t:\tTOKEN : INT\n");
        break;
      case LONG:
        printf("long\t:\tTOKEN : LONG\n");
        break;
      case FLOAT:
        printf("float\t:\tTOKEN : FLOAT\n");
        break;
      case DOUBLE:
        printf("double\t:\tTOKEN : double\n");
        break;
      case SIGNED:
        printf("signed\t:\tTOKEN : SIGNED\n");
        break;
      case UNSIGNED:
        printf("unsigned\t:\tTOKEN : UNSIGNED\n");
        break;
      case STRING:
        printf("string\t:\tTOKEN : STRING\n");
        break;
      case OBJECT:
        printf("object\t:\tTOKEN : OBJECT\n");
        break;
      case FUNC:
        printf("func\t:\tTOKEN : FUNC\n");
        break;
      case PASS:
        printf("pass\t:\tTOKEN : PASS\n");
        break;
      case RETURN:
        printf("return\t:\tTOKEN : RETURN\n");
        break;
      case O_PREN:
        printf("(\t:\tTOKEN : O_PREN\n");
        break;
      case C_PREN:
        printf(")\t:\tTOKEN : C_PREN\n");
        break;
      case O_SCOPE:
        printf("{\t:\tTOKEN : O_SCOPE\n");
        break;
      case C_SCOPE:
        printf("}\t:\tTOKEN : C_SCOPE\n");
        break;
      case SEMI_COLN:
        printf(";\t:\tTOKEN : SEMI_COLN\n");
        break;
      case COLN:
        printf(":\t:\tTOKEN : COLN\n");
        break;
      case EXPECT:
        printf("?\t:\tTOKEN : EXPECT\n");
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