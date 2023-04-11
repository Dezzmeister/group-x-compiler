#ifndef SRC_PARSEUTILS_H
#define SRC_PARSEUTILS_H

#include "parser.h"

#define MAX_STR_LEN     2048

#define yyerror(msg) x::syntax_error(msg)

extern char * yytext;
extern FILE * yyin;

extern int yylex();
void add_string(const char * str);

extern int lineno;

namespace x {
    /**
     * Raises a syntax error and stops the program. This should only
     * be called by the lexer or parser.
     */
    void syntax_error(const char * const msg);
}

#endif
