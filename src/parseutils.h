#ifndef SRC_PARSEUTILS_H
#define SRC_PARSEUTILS_H

#include "ast.h"
#include "parser.h"

#define MAX_STR_LEN     2048

extern char * yytext;
extern FILE * yyin;

extern int yylex();
extern int yylen;

namespace x {
    extern int lineno;

    /**
     * The top level node after parsing. Will be nullptr until the parser is finished.
     */
    extern ProgramSource * top;
}

#endif
