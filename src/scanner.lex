%option warn
%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "symtable.h"

int x::lineno = 1;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
%}

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
                    yylval.str_literal = new StringLiteral(str + 1);
                    free(str);
                    return STR;
                }
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
not         {return NOT_KW;}
in          {return IN_KW;}
not[ \t]+in {return NOT_IN_KW;}
and         {return AND_KW;}
or          {return OR_KW;}

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

\'\\n\'     {yylval.char_literal = new CharLiteral('\n'); return CHAR;}
\'\\t\'     {yylval.char_literal = new CharLiteral('\t'); return CHAR;}
\'\\0\'     {yylval.char_literal = new CharLiteral('\0'); return CHAR;}
\'\\e\'     {yylval.char_literal = new CharLiteral('\e'); return CHAR;}
\'\\r\'     {yylval.char_literal = new CharLiteral('\r'); return CHAR;}
\'.\'       {yylval.char_literal = new CharLiteral(yytext[1]); return CHAR;}

%%

void yyerror(const char * const error) {
    fprintf(stderr, "%s\nyytext: %s, line: %d\n", error, yytext, x::lineno);
    exit(1);
}

#pragma GCC diagnostic pop
