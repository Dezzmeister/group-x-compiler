#include "parseutils.h"

#include "parsedecls.h"

static SymbolTable * the_default_symtable = nullptr;

const char * BUILTIN_DECLS = R"(
    // These functions need stub definitions because a statement list cannot
    // be empty
    void print(char* str) { int _x; };
    char* i_to_str(int i) { int _x; };
    int calloc(int i)  { int _x; };
)";

ParseResult::ParseResult(int error, ParserState * parser_state) : error(error), parser_state(parser_state) {}

ParseResult::~ParseResult() {
    delete parser_state;
}

ParseResult x::parse_file(const char * const path) {
    FILE * yyin = fopen(path, "r");

    if (! yyin) {
        return ParseResult(errno, nullptr);
    }

    ParserState * state = new ParserState(std::string(path));
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
    ParserState * state = new ParserState(std::string("<stdin>"));
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

ParseResult x::parse_str(const char * code) {
    ParserState * state = new ParserState(std::string("<str>"));
    yyscan_t scanner;
    int error = yylex_init_extra(state, &scanner);

    if (error) {
        return ParseResult(error, nullptr);
    }

    YY_BUFFER_STATE buf = yy_scan_string(code, scanner);

    error = yyparse(scanner, state);

    if (error) {
        return ParseResult(error, state);
    }

    yy_delete_buffer(buf, scanner);

    error = yylex_destroy(scanner);

    return ParseResult(error, state);
}

SymbolTable * x::default_symtable() {
    if (the_default_symtable == nullptr) {
        the_default_symtable = x::bare_symtable();
        return the_default_symtable;
    }

    return the_default_symtable->clone();
}

void x::setup_symtable() {
    ParseResult result = x::parse_str(BUILTIN_DECLS);
    the_default_symtable = result.parser_state->symtable->clone();
}