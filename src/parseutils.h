#ifndef SRC_PARSEUTILS_H
#define SRC_PARSEUTILS_H

#include "ast.h"

#define MAX_STR_LEN 2048

typedef void* yyscan_t;

struct ParserState {
    // Line number (to be used while parsing)
    // TODO: Enable Bison locations and use them for error reporting
    size_t lineno;
    // Top level node of AST
    ProgramSource * top;

    ParserState();
};

int yylex_init_extra(ParserState * parser_state, yyscan_t * scanner);
int yylex_destroy(yyscan_t scanner);
extern void yyset_in(FILE * file, yyscan_t scanner);

typedef struct ParserState ParserState;

#endif
