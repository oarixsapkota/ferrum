#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

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