#ifndef SRC_PARSEDECLS_H
#define SRC_PARSEDECLS_H

#include "ast.h"
#include "symtable.h"

#define MAX_STR_LEN 2048

typedef void* yyscan_t;

struct ParserState {
    // Line number (to be used while parsing)
    // TODO: Enable Bison locations and use them for error reporting
    size_t lineno;
    // Top level node of AST
    ProgramSource * top;

    SymbolTable * symtable;

    ParserState();

    ~ParserState();
};

int yylex_init_extra(ParserState * state, yyscan_t * scanner);
int yylex_destroy(yyscan_t scanner);
int yyparse(yyscan_t scanner, ParserState * state);
extern void yyset_in(FILE * file, yyscan_t scanner);

typedef struct ParserState ParserState;

#endif
