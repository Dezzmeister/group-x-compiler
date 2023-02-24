#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

void x::syntax_error(const char * const msg) {
    fprintf(stderr, "Syntax error on line %d: %s\n", lineno, msg);
    exit(EXIT_FAILURE);
}
