#include "inc/lexer.h"

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

char *get_digit(const char *buffer, u64 *index, u64 line_num, uint *char_num, uint *is_float) {
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

void push_token(Token *token_head, Token **token_tail, Token_Type type, char *value) {
  Token *new_token = malloc(sizeof(Token));
  if (!new_token) {
    fprintf(stderr, "Memory allocation failed.\n");
    free(token_head);
    exit(EXIT_FAILURE);
  }
  new_token->type = type;
  new_token->value = value;
  new_token->next = NULL;

  if (*token_tail) {
    (*token_tail)->next = new_token;
  }
  *token_tail = new_token;
}

Token *lexer(const char *buffer) {
  u64 index = 0;
  u64 line_num = 1;
  uint char_num = 1;
  Token *head = malloc(sizeof(Token));
  if (!head) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  head->type = SOT;
  head->value = NULL;
  head->next = NULL;

  Token *token_tail = head;
  while (buffer[index] != '\0') {
    if (buffer[index] == '\n') {
      line_num++;
      char_num = 1;
      index++;
    } else if (isspace(buffer[index])) {
      index++;
      char_num++;
    } else if (isdigit(buffer[index]) || (buffer[index] == '.' && isdigit(buffer[index + 1]))) {
      uint is_float;
      char *digit = get_digit(buffer, &index, line_num, &char_num, &is_float);
      Token_Type type = (is_float == 1) ? FLOAT_LIT : INT_LIT;
      if (is_float >= 3)
        type = INT_LIT;
      push_token(head, &token_tail, type, digit);
    } else if (isalpha(buffer[index]) || buffer[index] == '_') {
      char *word = get_word(buffer, &index, &char_num);
      Token_Type type = get_word_type(word);
      if (type != IDENTIFIER) {
        free(word);
        word = NULL;
        push_token(head, &token_tail, type, word);
      } else if (type == IDENTIFIER) {
        if (buffer[index] == ':') {
          push_token(head, &token_tail, LABEL, word);
          index++;
          char_num++;
        } else {
          push_token(head, &token_tail, type, word);
        }
      }
    } else if (buffer[index] == '\'') {
      char *char_lit = get_char_lit(buffer, &index, &char_num, line_num);
      push_token(head, &token_tail, CHAR_LIT, char_lit);
    } else if (buffer[index] == '\"') {
      char *str_lit = get_str_lit(buffer, &index, &char_num, line_num);
      push_token(head, &token_tail, STRING_LIT, str_lit);
    } else {
      switch (buffer[index]) {
        case '/':
          if (buffer[index + 1] == '/') {
            while (buffer[index] != '\n')
              index++;
          } else if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, DEV_EQ, NULL);
            index++;
            char_num++;
          } else {
            push_token(head, &token_tail, DEV, NULL);
            index++;
            char_num++;
          }
          break;

        case '=':
          if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, EQUAL, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, ASSIGN, NULL);
          index++;
          char_num++;
          break;
        case '>':
          if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, GREATER_EQ, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, GREATER, NULL);
          index++;
          char_num++;
          break;
        case '<':
          if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, SMALLER_EQ, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, SMALLER, NULL);
          index++;
          char_num++;
          break;

        case '+':
          if (buffer[index + 1] == '+') {
            push_token(head, &token_tail, INCRIMENT, NULL);
            index++;
            char_num++;
          } else if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, ADD_EQ, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, ADD, NULL);
          index++;
          char_num++;
          break;
        case '-':
          if (buffer[index + 1] == '-') {
            push_token(head, &token_tail, DECRIMENT, NULL);
            index++;
            char_num++;
          } else if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, SUB_EQ, NULL);
            index++;
            char_num++;
          } else if (buffer[index + 1] == '>') {
            push_token(head, &token_tail, ARROW, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, SUB, NULL);
          index++;
          char_num++;
          break;
        case '*':
          if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, MUL_EQ, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, MUL, NULL);
          index++;
          char_num++;
          break;
        case '%':
          if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, MOD_EQ, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, MOD, NULL);
          index++;
          char_num++;
          break;

        case '!':
          if (buffer[index + 1] == '=') {
            push_token(head, &token_tail, NOT_EQ, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, NOT, NULL);
          index++;
          char_num++;
          break;

        case '&':
          if (buffer[index + 1] == '&') {
            push_token(head, &token_tail, AND, NULL);
            index++;
            char_num++;
          } else
            push_token(head, &token_tail, ADDRESS_OF, NULL);
          index++;
          char_num++;
          break;
        case '|':
          if (buffer[index + 1] == '|') {
            push_token(head, &token_tail, OR, NULL);
            index++;
            char_num++;
          } else
            fprintf(stderr, "Invalid symbol `%c' at line %lld char %d\n", buffer[index], line_num, char_num);
          index++;
          char_num++;
          break;

        case '.':
          push_token(head, &token_tail, DOT, NULL);
          index++;
          char_num++;
          break;
        case '^':
          push_token(head, &token_tail, POINTER, NULL);
          index++;
          char_num++;
          break;

        case '(':
          push_token(head, &token_tail, O_PREN, NULL);
          index++;
          char_num++;
          break;
        case ')':
          push_token(head, &token_tail, C_PREN, NULL);
          index++;
          char_num++;
          break;
        case '[':
          push_token(head, &token_tail, O_BBRACK, NULL);
          index++;
          char_num++;
          break;
        case ']':
          push_token(head, &token_tail, C_BBRACK, NULL);
          index++;
          char_num++;
          break;
        case '{':
          push_token(head, &token_tail, O_SCOPE, NULL);
          index++;
          char_num++;
          break;
        case '}':
          push_token(head, &token_tail, C_SCOPE, NULL);
          index++;
          char_num++;
          break;
        case ':':
          push_token(head, &token_tail, COLN, NULL);
          index++;
          char_num++;
          break;
        case ';':
          push_token(head, &token_tail, SEMI_COLN, NULL);
          index++;
          char_num++;
          break;
        case '?':
          push_token(head, &token_tail, EXPECT, NULL);
          index++;
          char_num++;
          break;
        case ',':
          push_token(head, &token_tail, COMMA, NULL);
          index++;
          char_num++;
          break;

        case '@':
          push_token(head, &token_tail, DIRECTIVE, NULL);
          index++;
          char_num++;
          break;


        default:
          fprintf(stderr, "Invalid symbol `%c' at line %lld char %d\n", buffer[index], line_num, char_num);
          while (isspace(buffer[index]))
            index++;
          char_num++;
          index++;
          break;
      }
    }
  }
  push_token(head, &token_tail, EOT, NULL);
  return head;
}

void print_tokens(Token *token) {
  while (token) {
    Token *next = token->next;
    switch (token->type) {
      case 0:
        printf("UNKNOWN TOKEN\t:\t%s\n", token->value);
        break;

      case SOT:
        printf("####\tTOKEN START\t####\n");
        break;
      case EOT:
        printf("####\tTOKEN END\t####\n");
        break;

      case IDENTIFIER:
        printf("IDENTIFIER\tVALUE : %s\n", token->value);
        break;
      case LABEL:
        printf("LABLE\tVALUE : %s\n", token->value);
        break;

      case INT_LIT:
        printf("INT_LIT  \tVALUE : %s\n", token->value);
        break;
      case FLOAT_LIT:
        printf("FLOAT_LIT\tVALUE : %s\n", token->value);
        break;
      case CHAR_LIT:
        printf("CHAR_LIT\tVALUE : %s\n", token->value);
        break;
      case STRING_LIT:
        printf("STRING_LIT\tVALUE : %s\n", token->value);
        break;

      case VOID:
        printf("VOID\n");
        break;
      case NULL_C:
        printf("NULL_C\n");
        break;
      case NULL_S:
        printf("NULL_S\n");
        break;
      case BOOL:
        printf("BOOL\n");
        break;
      case TRUE:
        printf("TRUE\n");
        break;
      case FALSE:
        printf("FALSE\n");
        break;
      case CHAR:
        printf("CHAR\n");
        break;
      case SHORT:
        printf("SHORT\n");
        break;
      case INT:
        printf("INT\n");
        break;
      case LONG:
        printf("LONG\n");
        break;
      case FLOAT:
        printf("FLOAT\n");
        break;
      case DOUBLE:
        printf("double\n");
        break;
      case SIGNED:
        printf("SIGNED\n");
        break;
      case UNSIGNED:
        printf("UNSIGNED\n");
        break;
      case STRING:
        printf("STRING\n");
        break;
      case OBJECT:
        printf("OBJECT\n");
        break;

      case FUNC:
        printf("FUNC\n");
        break;
      case PASS:
        printf("PASS\n");
        break;
      case RETURN:
        printf("RETURN\n");
        break;

      case ADD:
        printf("ADD\n");
        break;
      case SUB:
        printf("SUB\n");
        break;
      case MUL:
        printf("MUL\n");
        break;
      case DEV:
        printf("DEV\n");
        break;
      case MOD:
        printf("MOD\n");
        break;

      case ASSIGN:
        printf("ASSIGN\n");
        break;
      case ADD_EQ:
        printf("ADD_EQ\n");
        break;
      case SUB_EQ:
        printf("SUB_EQ\n");
        break;
      case MUL_EQ:
        printf("MUL_EQ\n");
        break;
      case DEV_EQ:
        printf("DEV_EQ\n");
        break;
      case MOD_EQ:
        printf("MOD_EQ\n");
        break;

      case GREATER:
        printf("GREATER\n");
        break;
      case SMALLER:
        printf("SMALLER\n");
        break;
      case GREATER_EQ:
        printf("GREATER_EQ\n");
        break;
      case SMALLER_EQ:
        printf("SMALLER_EQ\n");
        break;
      case EQUAL:
        printf("EQUAL\n");
        break;
      case NOT_EQ:
        printf("NOT_EQ\n");
        break;

      case NOT:
        printf("NOT\n");
        break;
      case AND:
        printf("AND\n");
        break;
      case OR:
        printf("OR\n");
        break;

      case DECRIMENT:
        printf("DECRIMENT\n");
        break;
      case INCRIMENT:
        printf("INCRIMENT\n");
        break;

      case POINTER:
        printf("POINTER\n");
        break;
      case ADDRESS_OF:
        printf("ADDRESS_OF\n");
        break;
      case DOT:
        printf("DOT\n");
        break;
      case ARROW:
        printf("ARROW\n");
        break;

      case O_PREN:
        printf("O_PREN\n");
        break;
      case C_PREN:
        printf("C_PREN\n");
        break;
      case O_BBRACK:
        printf("O_BBRACK\n");
        break;
      case C_BBRACK:
        printf("C_BBRACK\n");
        break;
      case O_SCOPE:
        printf("O_SCOPE\n");
        break;
      case C_SCOPE:
        printf("C_SCOPE\n");
        break;
      case SEMI_COLN:
        printf("SEMI_COLN\n");
        break;
      case COLN:
        printf("COLN\n");
        break;
      case EXPECT:
        printf("EXPECT\n");
        break;
      case COMMA:
        printf("COMMA\n");
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