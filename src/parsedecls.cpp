#include "parsedecls.h"

#include <vector>

ParserState::ParserState(std::string current_source)
    :   top(nullptr),
        symtable(x::default_symtable()),
        errors(ErrorReport()),
        current_errors(SourceErrors()),
        current_source(current_source),
        debug_stmts(std::map<int, Statement *>())
{}

ParserState::~ParserState() {
    delete top;
    delete symtable;
}
