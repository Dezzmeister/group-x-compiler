#include <stdio.h>
#include <stdlib.h>
#include "src/parseutils.h"
#include "src/symtable.h"
#include <unistd.h>

extern int yydebug;

#define MAX_STR_NUM 100

const char *str_literals[MAX_STR_NUM];

void add_string(const char * str) {
  static int i = 0;
  str_literals[i++] = str;
}

extern SymbolTable * symtable;

int main(int argc, char ** argv) {
    if (YYDEBUG) { yydebug = 1; }  /* Enable tracing */

    /*
    * Print hello world with make, then ./release_bin examples/hello.x
    */

    if (argc == 2) {
      yyin = fopen(argv[1], "r");
      if (!yyin) {
          fprintf(stderr, "Cannot open file %s for reading.\n", argv[1]);
          exit(1);
      }
        close(0);
        if(dup(fileno(yyin))) {
          fprintf(stderr, "dup error.\n");
          exit(1);
        }
    } 
    yyparse();
}
