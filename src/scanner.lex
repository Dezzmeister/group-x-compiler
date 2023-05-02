%option reentrant bison-bridge bison-locations warn noyywrap

%{
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "symtable.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

// Update the location object for each token
static void yy_user_action(YYLTYPE * loc, char * txt) {
    loc->first_line = loc->last_line;
    loc->first_column = loc->last_column;
    for (int i = 0; txt[i] != '\0'; i++) {
        if (txt[i] == '\n') {
            loc->last_line++;
            loc->last_column = 0;
        } else {
            loc->last_column++;
        }
    }
}

#define YY_USER_ACTION yy_user_action(yylloc, yytext);
%}

%option extra-type="ParserState*"

str_literal \"[^\"\n]*\"
delim       [ \t]
ws          {delim}+
int         [[:digit:]]+
hexint      (\-)?0[xX][[:xdigit:]]+
float       (\-)?[[:digit:]]+\.[[:digit:]]*f?
bool        (true)|(false)
letter      [a-zA-Z]

ident       ({letter}|_)({letter}|[[:digit:]]|_)*

comment     \/\/.*\n

%%
{str_literal}   {
                    char * str = strdup(yytext);
                    int len = strlen(str);
                    // Remove quotes
                    str[len - 1] = '\0';
                    yylval->str_literal = new StringLiteral(Location(*yylloc, *yylloc), str + 1);
                    free(str);
                    return STR;
                }
{ws}        {}
{comment}   {}
\n          {}
{int}       {yylval->int_literal = new IntLiteral(Location(*yylloc, *yylloc), yytext); return INT;}
{float}     {yylval->float_literal = new FloatLiteral(Location(*yylloc, *yylloc), yytext); return FLOAT;}
true        {yylval->bool_literal = new BoolLiteral(Location(*yylloc, *yylloc), true); return BOOL;}
false       {yylval->bool_literal = new BoolLiteral(Location(*yylloc, *yylloc), false); return BOOL;}

type        {return TYPE_ALIAS_KW;}
struct      {return STRUCT_KW;}
as          {return CAST_KW;}
return      {return RETURN_KW;}
if          {return IF_KW;}
else        {return ELSE_KW;}
while       {return WHILE_KW;}
for         {return FOR_KW;}

mut         {return MUT;}
not         {return NOT_KW;}
in          {return IN_KW;}
not[ \t]+in {return NOT_IN_KW;}
and         {return AND_KW;}
or          {return OR_KW;}

{ident}     {
                yylval->ident = new Ident(Location(*yylloc, *yylloc), yytext); 
                // Our grammar is not context free because of this: the lexer returns
                // a different token depending on whether the identifier has been declared
                // as a type, variable, or function, or if it's undeclared. This is great because
                // it lets us do more with the grammar without running into conflicts
                const Symbol * sym = yyextra->symtable->get(std::string(yytext));

                if (sym != nullptr) {
                    if (sym->kind == Var) {
                        return DECLARED_VAR;
                    } else if (sym->kind == Type) {
                        delete yylval->ident;
                        yylval->type_ident = new TypeIdent(Location(*yylloc, *yylloc), yytext);
                        return DECLARED_TYPE;
                    }

                    return DECLARED_FUNC;
                }

                return IDENT;
            }

\-\>        {return FUNC_TYPE_OP;}

"&&"        {return BOOL_AND;}
"||"        {return BOOL_OR;}

"=="        {return EQU;}
"!="        {return NEQ;}
">"         {return GTR;}
"<"         {return LES;}
">="        {return GEQ;}
"<="        {return LEQ;}

"++"        {return INC;}
"--"        {return DEC;}

\+          {return '+';}
\-          {return '-';}
\/          {return '/';}
\*          {return '*';}
\(          {return '(';}
\)          {return ')';}
\,          {return ',';}
\[          {return '[';}
\]          {return ']';}
\=          {return '=';}
;           {return ';';}
\{          {return '{';}
\}          {return '}';}
&           {return '&';}
%           {return '%';}
!           {return '!';}

\'\\n\'     {yylval->char_literal = new CharLiteral(Location(*yylloc, *yylloc), '\n'); return CHAR;}
\'\\t\'     {yylval->char_literal = new CharLiteral(Location(*yylloc, *yylloc), '\t'); return CHAR;}
\'\\0\'     {yylval->char_literal = new CharLiteral(Location(*yylloc, *yylloc), '\0'); return CHAR;}
\'\\e\'     {yylval->char_literal = new CharLiteral(Location(*yylloc, *yylloc), '\e'); return CHAR;}
\'\\r\'     {yylval->char_literal = new CharLiteral(Location(*yylloc, *yylloc), '\r'); return CHAR;}
\'.\'       {yylval->char_literal = new CharLiteral(Location(*yylloc, *yylloc), yytext[1]); return CHAR;}

@           {
                #ifndef DEBUG_TOKENS
                    ECHO;
                #else
                    return DEBUG_TOKEN;
                #endif
            }

<<EOF>>     {return END;}

%%

void yyerror(YYLTYPE * loc, yyscan_t scanner, ParserState * state, char const *format, ...) {
    fprintf(stderr, "line: %d\n", loc->first_line);
    va_list args;
    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
    putc ('\n', stderr);
    exit(1);
}

#pragma GCC diagnostic pop
