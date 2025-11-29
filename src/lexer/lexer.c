#include "lexer.h"

#include "../inc/ferrum.h"
#include "literal.c"
#include "token.c"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

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
      char *digit = get_digit(buffer, &index, &char_num, line_num, &is_float);
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
    } else if (buffer[index] == 39) { // 39 = ' for char literal
      char *char_lit = get_char_lit(buffer, &index, &char_num, line_num);
      push_token(head, &token_tail, CHAR_LIT, char_lit);
    } else if (buffer[index] == 34) { // 34 = " for string literal
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
