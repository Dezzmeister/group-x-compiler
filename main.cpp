#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int val = yyparse();
    printf("status code: %d\n", val);
    printf("expr: ");
    x::top->print();
    printf("\nkind: %s\n", x::kind_map[x::top->get_kind()].c_str());

    return val;
}
