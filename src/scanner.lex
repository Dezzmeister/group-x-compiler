%option warn

%{
#include "parseutils.h"
#include "parser.h"
#include "symtable.h"
// TODO: Circular imports can break the scanner. Include a mechanism to prevent
// this

// TODO: Return string buf to parser
static char string_buf[MAX_STR_LEN] = { 0 };
static char * string_buf_ptr;

int lineno = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
%}
%x str
%x incl

delim       [ \t]
ws          {delim}+
digit       [0-9]
int         (\-)?([[:digit:]]{-}[0])[[:digit:]]*
hexint      (\-)?0[xX][[:xdigit:]]+
float       (\-)?{digit}+\.{digit}*f?
num         {int}|{hexint}|{float}
bool        (true)|(false)
letter      [a-zA-Z]

if       if
else     else
while    while
break    break
for      for
return   return
print    print
struct   struct

int_type   int
float_type float
char_type  char

id          {letter}({letter}|{digit})*
char        '((.)|(\\n)|(\\r)|(\\t)|(\\0))'

%%

\"          string_buf_ptr = string_buf; BEGIN(str);
import      BEGIN(incl);
<str>{
    \"      {
        BEGIN(INITIAL);
        *string_buf_ptr = '\0';
        return STR;
    }
    \n      {
        x::syntax_error("Malformed string");
    }
    \\n     *string_buf_ptr++ = '\n';
    \\t     *string_buf_ptr++ = '\t';
    \\r     *string_buf_ptr++ = '\r';
    \\(.|\n)    *string_buf_ptr++ = yytext[1];
    [^\\\n\"]+  {
        char *yptr = yytext;

        while (*yptr) {
            *string_buf_ptr++ = *yptr++;
        }
    }
}
<incl>{
    [ \t]*      {}
    [^ \t\n]+   {
        yyin = fopen(yytext, "r");

        if (! yyin) {
            char s[50];
            sprintf(s, "Nonexistent or unreadable file %s", yytext);
            x::syntax_error(s);
        }
        yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

        BEGIN(INITIAL);
    }
}
<<EOF>> {
    yypop_buffer_state();
    if (! YY_CURRENT_BUFFER) {
        yyterminate();
    }
}

{ws}        {}
\n          {lineno++; return NEWLINE;}

{num}       {yylval.itype = atoi(yytext); return NUM;} 
{bool}      {return BOOL;}
{char}      {yylval.ctype = *yytext; return CHAR;}

{int_type}    {return INT_TYPE;}
{float_type}  {return FLOAT_TYPE;}
{char_type}   {return CHAR_TYPE;}

{if}         {return IF;}
{else}       {return ELSE;}
{while}      {return WHILE;}
{break}      {return BREAK;}
{for}        {return FOR;}
{return}     {return RETURN;}
{print}      {return PRINT;}
{struct}     {return STRUCT;}

{id}    {
        struct symrec* s = getsym(yytext);
        if (!s) {
            char new_str[strlen(yytext) + 1];
            strcpy(new_str,yytext);
            s = putsym(new_str, ID); 
        }
        yylval.string = yytext;
        return ID;
        }

"="         {return ASSIGN;}
[\(\)]      {return *yytext;}
[+-\/*\^]   {return *yytext;}



%%

#pragma GCC diagnostic pop
