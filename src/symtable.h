#ifndef SYMTABLE_H
#define SYMTABLE_H
#include "parser.h"
union YYSTYPE;
typedef struct symrec {
    char *name;  
    int type;    
    YYSTYPE val;
    struct symrec *next;  
    } symrec;

extern symrec *sym_table;

symrec *putsym (char const *name, int type);
symrec *getsym (char const *name);
#endif
