#include <stdio.h>
#include <stdlib.h>
#include "src/parseutils.h"

symrec *sym_table;

symrec * putsym (char const *name)
{
  symrec *res = (symrec *) malloc (sizeof (symrec));
  res->name = strdup(name);
  res->next = sym_table;
  sym_table = res;
  return res;
}

symrec * getsym (char const *name)
{
  for (symrec *p = sym_table; p; p = p->next)
    if (strcmp (p->name, name) == 0)
      return p;
  return nullptr;
}

int main() {
    // if (YYDEBUG) { yydebug = 1; }  /* Enable tracing */
    /*
    * The parse tree currently only recognizes arithmetic expressions.
    * To test run make debug then ./debug_bin and enter an arithmetic 
    * experssion Press Ctrl-D to signal end of input.
    */
    return yyparse();
}
