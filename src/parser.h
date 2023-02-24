#ifndef PARSER_H
#define PARSER_H

// 100, 0xdeadbeef, -50.4, 79.3f, -0x49e, -4., ...
#define NUM_LITERAL     1
// 'e', ' ', '\n', ...
#define CHAR_LITERAL    2
// "str with escapes\n", "", ...
#define STR_LITERAL     3
// true, false
#define BOOL_LITERAL    4
// x, y, var_name, func_name, ...
#define ID              5

#define LINE_SEP        6

#define IF_KW           100
#define ELSE_KW         101
#define WHILE_KW        102
#define BREAK_KW        103
#define FOR_KW          104
#define INT_KW          105
#define FLOAT_KW        106

#define MAX_STR_LEN     2048

extern char * yytext;

extern int yylex();

extern int lineno;

namespace x {
    /**
     * Raises a syntax error and stops the program. This should only
     * be called by the lexer or parser.
     */
    void syntax_error(const char * const msg);
}

#endif
