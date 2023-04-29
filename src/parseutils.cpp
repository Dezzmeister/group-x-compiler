#include "parseutils.h"

#include "parsedecls.h"

ParseResult::ParseResult(int error, ParserState * parser_state) : error(error), parser_state(parser_state) {}

ParseResult::~ParseResult() {
    delete parser_state;
}

ParseResult x::parse_file(const char * const path) {
    FILE * yyin = fopen(path, "r");

    if (! yyin) {
        return ParseResult(errno, nullptr);
    }

    ParserState * state = new ParserState();
    yyscan_t scanner;
    int error = yylex_init_extra(state, &scanner);

    yyset_in(yyin, scanner);

    if (error) {
        return ParseResult(error, nullptr);
    }

    error = yyparse(scanner, state);

    if (error) {
        return ParseResult(error, state);
    }

    error = yylex_destroy(scanner);

    if (error) {
        return ParseResult(error, state);
    }

    error = fclose(yyin) ? errno : 0;

    return ParseResult(error, state);
}

ParseResult x::parse_stdin() {
    ParserState * state = new ParserState();
    yyscan_t scanner;
    int error = yylex_init_extra(state, &scanner);

    if (error) {
        return ParseResult(error, nullptr);
    }

    error = yyparse(scanner, state);

    if (error) {
        return ParseResult(error, state);
    }

    error = yylex_destroy(scanner);

    return ParseResult(error, state);
}