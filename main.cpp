#include <stdio.h>
#include <stdlib.h>

#include "src/parseutils.h"

int main() {
    int c;
    while ((c = yylex())) {
        printf("yylex: %d\n", c);
    }

    printf("num literal id: %d\n", NUM_LITERAL);

    return EXIT_SUCCESS;
}
