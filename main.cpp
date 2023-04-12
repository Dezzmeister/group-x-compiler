#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/parseutils.h"
#include "src/symtable.h"

symrec *sym_table;

symrec * putsym (char const *name, int type)
{
  symrec *res = (symrec *) malloc (sizeof (symrec));
  res->name = strdup(name);
  res->type = type;
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
    int val = yyparse();
    printf("status code: %d\n", val);
    printf("expr: ");
    x::top->print();
    printf("\nkind: %s\n", x::kind_map[x::top->get_kind()].c_str());

    return val;
}
