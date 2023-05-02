#include "errors.h"

CompilerError::CompilerError(Location loc, const char * const message, ErrorLevel level)
    : loc(loc), message(std::string(message)), level(level) {}

CompilerError::CompilerError(Location loc, std::string message, ErrorLevel level)
    : loc(loc), message(message), level(level) {}

void CompilerError::print(FILE * output) {
    const char * const lvl = level == Warn ? "Warning" : "Error";
    fprintf(output, "%s (%d:%d to %d:%d): %s\n", lvl, loc.first_line, loc.first_col, loc.last_line, loc.last_col,
            message.c_str());
}

SourceErrors::SourceErrors() : parse_errors({}), type_errors({}) {}

ErrorReport::ErrorReport() : sources(std::map<ProgramSource *, SourceErrors>()) {}
