%{
#include "parser.h"

static char string_buf[MAX_STR_LEN] = { 0 };
static char * string_buf_ptr;
int lineno = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
%}
%option yylineno
%x str

delim       [ \t]
ws          {delim}+
digit       [0-9]
int         (\-)?{digit}+
hexint      (\-)?0x({digit}|[a-fA-F])+
float       (\-)?{digit}+\.{digit}*f?
num         {int}|{hexint}|{float}
bool        (true)|(false)
letter      [a-zA-Z]
if_kw       if
else_kw     else
while_kw    while
break_kw    break
for_kw      for
int_kw      int
float_kw    float
id          {letter}({letter}|{digit})*
char        '((.)|(\\n)|(\\r)|(\\t)|(\0))'

%%

\"          string_buf_ptr = string_buf; BEGIN(str);
<str>{
    \"      {
        BEGIN(INITIAL);
        *string_buf_ptr = '\0';
        return STR_LITERAL;
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

{ws}        {}
\n          {lineno++;}
{num}       {return NUM_LITERAL;}
{bool}      {return BOOL_LITERAL;}
{char}      {return CHAR_LITERAL;}

{if_kw}     {return IF_KW;}
{else_kw}   {return ELSE_KW;}
{while_kw}  {return WHILE_KW;}
{break_kw}  {return BREAK_KW;}
{for_kw}    {return FOR_KW;}
{int_kw}    {return INT_KW;}
{float_kw}  {return FLOAT_KW;}

{id}        {return ID;}

%%

#pragma GCC diagnostic pop
