%option warn

%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "symtable.h"

// static char string_buf[MAX_STR_LEN] = { 0 };
// static char * string_buf_ptr;

int x::lineno = 1;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
%}
%x str

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
{str_literal}  {yylval.c_str = strdup(yytext);}
{ws}        {}
{comment}   {x::lineno++;}
\n          {x::lineno++;}
{int}       {yylval.int_literal = new IntLiteral(yytext); return INT;}
{float}     {yylval.float_literal = new FloatLiteral(yytext); return FLOAT;}
true        {yylval.bool_literal = new BoolLiteral(true); return BOOL;}
false       {yylval.bool_literal = new BoolLiteral(false); return BOOL;}

type        {return TYPE_ALIAS_KW;}
struct      {return STRUCT_KW;}
as          {return CAST_KW;}
return      {return RETURN_KW;}
if          {return IF_KW;}
else        {return ELSE_KW;}
while       {return WHILE_KW;}
for         {return FOR_KW;}

mut         {return MUT;}

{ident}     {
                yylval.ident = new Ident(yytext); 
                // Our grammar is not context free because of this: the lexer returns
                // a different token depending on whether the identifier has been declared
                // as a type, variable, or function, or if it's undeclared. This is great because
                // it lets us do more with the grammar without running into conflicts
                const Symbol * sym = x::symtable->get(std::string(yytext));

                if (sym != nullptr) {
                    if (sym->type == Var) {
                        return DECLARED_VAR;
                    } else if (sym->type == Type) {
                        delete yylval.ident;
                        yylval.type_ident = new TypeIdent(yytext);
                        return DECLARED_TYPE;
                    }

                    return DECLARED_FUNC;
                }

                return IDENT;
            }

\-\>        {return FUNC_TYPE_OP;}

"&&"        {return BOOL_AND;}
"||"        {return BOOL_OR;}

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

%%

void yyerror(const char * const error) {
    fprintf(stderr, "%s\nyytext: %s, line: %d\n", error, yytext, x::lineno);
    exit(1);
}

#pragma GCC diagnostic pop
