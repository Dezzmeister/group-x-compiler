%{
#include "parser.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
%}

delim       [ \t]
ws          {delim}+
digit       [0-9]
num         {digit}+

%%

{ws}        {}
{num}       {return NUM_LITERAL;}

%%

#pragma GCC diagnostic pop
