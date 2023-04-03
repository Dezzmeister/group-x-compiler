#ifndef TREE_H
#define TREE_H
#include "stdio.h"
#include <cstring>
#include "parser.h"

union YYSTYPE;
typedef struct tree {
        int type;
        YYSTYPE val;
        struct tree * left;
        struct tree * right;
} tree;

tree * mktree(int type, tree *left, tree *right);
tree * mktree(int type, YYSTYPE y);

void print_tree(tree *root);
#endif 
