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

bool SourceErrors::has_errors() const {
    return (parse_errors.size() > 0 || type_errors.size() > 0);
}

int SourceErrors::error_count() const {
    return parse_errors.size() + type_errors.size();
}

ErrorReport::ErrorReport() : sources(std::map<ProgramSource *, SourceErrors>()) {}

void ErrorReport::print(FILE * output) {
    for (auto &item : sources) {
        if (item.second.has_errors()) {
            fprintf(output, "%s:\n", item.first->name.c_str());
        }

        for (auto &error : item.second.parse_errors) {
            fprintf(output, "\t");
            error.print(output);
        }

        for (auto &error : item.second.type_errors) {
            fprintf(output, "\t");
            error.print(output);
        }
    }
}
