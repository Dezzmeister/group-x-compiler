#include "errors.h"

CompilerError::CompilerError(Location loc, std::string message, ErrorLevel level)
    : loc(loc), message(message), level(level) {}

SourceErrors::SourceErrors() : parse_errors({}), type_errors({}) {}

ErrorReport::ErrorReport() : sources(std::map<ProgramSource *, SourceErrors>()) {}
