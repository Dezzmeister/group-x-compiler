#include "parsedecls.h"

#include <vector>

ParserState::ParserState() : lineno(1), top(nullptr), symtable(x::default_symtable()) {}

ParserState::~ParserState() {
    delete top;
    delete symtable;
}
