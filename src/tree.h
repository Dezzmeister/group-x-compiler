#ifndef TREE_H
#define TREE_H
#include "stdio.h"
#include <cstring>
typedef struct tree {
        int type;
        struct tree * left;
        struct tree * right;
} tree;

tree * mktree(int type, tree *left, tree *right);

void print_tree(tree *root);
#endif 
