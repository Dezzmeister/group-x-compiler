%option warn

%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

// static char string_buf[MAX_STR_LEN] = { 0 };
// static char * string_buf_ptr;

int x::lineno = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
%}
%x str

delim       [ \t]
ws          {delim}+
int         [[:digit:]]+
hexint      (\-)?0[xX][[:xdigit:]]+
float       (\-)?[[:digit:]]+\.[[:digit:]]*f?
bool        (true)|(false)
letter      [a-zA-Z]

if          if
else        else

ident       ({letter}|_)({letter}|[[:digit:]]|_)*

%%

{ws}        {}
\n          {x::lineno++; return NEWLINE;}
{int}       {yylval.int_literal = new IntLiteral(yytext); return INT;}
{float}     {yylval.float_literal = new FloatLiteral(yytext); return FLOAT;}

int         {yylval.ident = new Ident(yytext); return INT_TYPE;}
float       {yylval.ident = new Ident(yytext); return FLOAT_TYPE;}
char        {yylval.ident = new Ident(yytext); return CHAR_TYPE;}

type        {return TYPE_ALIAS_KW;}
struct      {return STRUCT_KW;}

mut         {return MUT;}

{ident}     {yylval.ident = new Ident(yytext); return IDENT;}

\-\>        {return FUNC_TYPE_OP;}

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

%%

int yywrap() {
    return 1;
}

void yyerror(const char * const error) {
    fprintf(stderr, "%s\nyytext: %s\n", error, yytext);
    exit(1);
}

#pragma GCC diagnostic pop
