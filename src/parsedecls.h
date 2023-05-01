#ifndef SRC_PARSEDECLS_H
#define SRC_PARSEDECLS_H

#include "ast.h"
#include "errors.h"
#include "symtable.h"

#define MAX_STR_LEN 2048

struct yy_buffer_state;
typedef struct yy_buffer_state * YY_BUFFER_STATE;
typedef void* yyscan_t;

struct ParserState {
    // Top level node of AST
    ProgramSource * top;

    SymbolTable * symtable;

    // Error report for all source files
    ErrorReport errors;

    // Error report for current source file
    SourceErrors current_errors;

    // Name of current source
    std::string current_source;

    ParserState(std::string current_source);

    ~ParserState();
};

// TODO: Use Bison locations for error reporting from lexer and parser
void yyerror(YYLTYPE * loc, yyscan_t scanner, ParserState * state, char const * format, ...);
int yylex_init_extra(ParserState * state, yyscan_t * scanner);
int yylex_destroy(yyscan_t scanner);
int yyparse(yyscan_t scanner, ParserState * state);
YY_BUFFER_STATE yy_scan_string(const char * yy_str, yyscan_t scanner);
void yy_delete_buffer(YY_BUFFER_STATE b, yyscan_t scanner);
extern void yyset_in(FILE * file, yyscan_t scanner);

typedef struct ParserState ParserState;

#endif
