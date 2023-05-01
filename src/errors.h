#ifndef SRC_ERRORS_H
#define SRC_ERRORS_H

#include <map>
#include <string>
#include <vector>

#include "ast.h"

enum ErrorLevel {
    Warn,
    Error
};

struct CompilerError {
    Location loc;
    std::string message;
    ErrorLevel level;

    CompilerError(Location loc, std::string message, ErrorLevel level);
};

typedef struct CompilerError CompilerError;

struct SourceErrors {
    std::vector<CompilerError> parse_errors;
    std::vector<CompilerError> type_errors;

    SourceErrors();
};

typedef struct SourceErrors SourceErrors;

struct ErrorReport {
    std::map<ProgramSource *, SourceErrors> sources;

    ErrorReport();
};

typedef struct ErrorReport ErrorReport;

#endif