#include "inc/utils.h"
#include "lexer/lexer.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  char *buffer = read_file("test.fe");
  if (buffer == NULL) {
    perror("ERROR WITH FILE :");
    printf(": FILE ERROR!!\n");
    return -1;
  }

  Token *tokens = lexer(buffer);

  free(buffer);

  print_tokens(tokens);

  free_tokens(tokens);

  return 0;
}

#include "lexer/lexer.c"
#include "utils.c"
