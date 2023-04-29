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
}


#endif