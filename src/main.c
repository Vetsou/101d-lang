#include <stdio.h>
#include "lexer.h"

int main(void) {
  printf("Hello world !!!\n");
  lexer_t lexer;
  lexer_init(&lexer, "ttt", 4);
  token_t tok = lexer_scan(&lexer);
  printf("%s", tok.start);
  return 0;
}