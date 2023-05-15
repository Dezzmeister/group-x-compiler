#ifndef SRC_ERRORS_H
#define SRC_ERRORS_H

#include <map>
#include <stdio.h>
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

    CompilerError(Location loc, const char * const message, ErrorLevel level);
    CompilerError(Location loc, std::string message, ErrorLevel level);

    void print(FILE * output);
};

typedef struct CompilerError CompilerError;

struct SourceErrors {
        std::vector<CompilerError> parse_errors;
    std::vector<CompilerError> type_errors;

    SourceErrors();

    bool has_errors() const;
    int error_count() const;
};

typedef struct SourceErrors SourceErrors;

struct ErrorReport {
    std::map<ProgramSource *, SourceErrors> sources;

    ErrorReport();

    void print(FILE * output);
};

typedef struct ErrorReport ErrorReport;

#endif