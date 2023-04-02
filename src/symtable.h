#ifndef SYMTABLE_H
#define SYMTABLE_H
typedef struct symrec {
    char *name;  
    int type;    
    struct symrec *next;  
    } symrec;

extern symrec *sym_table;

symrec *putsym (char const *name, struct val sym_val);
symrec *getsym (char const *name);
#endif
