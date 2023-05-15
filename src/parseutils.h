#ifndef SRC_PARSEUTILS_H
#define SRC_PARSEUTILS_H

#include "ast.h"
#include "parsedecls.h"
#include "symtable.h"

struct ParseResult {
    int error;
    ParserState * parser_state;

    ParseResult(int error, ParserState * parser_state);

    ~ParseResult();
};

typedef struct ParseResult ParseResult;

namespace x {
    ParseResult parse_file(const char * const path);

    ParseResult parse_stdin();

    ParseResult parse_str(const char * code);

    SymbolTable * default_symtable();

    void setup_symtable();
}


#endif