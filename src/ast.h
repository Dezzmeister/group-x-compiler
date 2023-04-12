#ifndef SRC_AST_H
#define SRC_AST_H
#include <stdio.h>
class ASTNode {
    public:
        virtual int get_kind() = 0;
        virtual void print() = 0;
        static void PrintASTNode(ASTNode * root)  {
            if (!root) {return;}
            root->print();
            putchar('\n');
        }
};

class Expr : public ASTNode {
    public:
    Expr() {}
};

class Stmt : public ASTNode {
    public:
    Stmt() {}
};

class ExprStmt : public Stmt {
    public:
    Expr * expr;
    ExprStmt(Expr * e) {
        this->expr = e;
    }
    int get_kind() {
        return 0;
    }
    void print() {
        expr->print();
    }
};

class ArithExpr : public Expr {
    public:
    char op;
    Expr * left;
    Expr * right;
    ArithExpr(char, Expr *, Expr *);
    int get_kind() { return 0; }
    void print() {
        left->print();
        putchar(op);
        right->print();
    }
};

class Id : public Expr {
    public:
    // Points to symbol table.
    const char * name;
    Id(const char * name) : name(name) {}
    void print() {
        printf("%s", name);
    }
    int get_kind() {
        return 0;
    }
};

class Statements : public Stmt {
    public:
    Stmt * left;
    Stmt * right;
    Statements(Stmt * left, Stmt * right);
    void print()
    {
        if (left)  {  left->print();  }
        if (right) {  right->print(); }
    }
    int get_kind() {
        return 0;
    }
};

class Print : public Stmt {
    public:
    Expr * expr;
    Print(Expr * e);
    const static int kind = 3;
    int get_kind() {
        return kind;
    }
    void print()
    {
        printf("(print ");
        expr->print();
        printf(") ");
    }
};

class If : public Stmt {
    public:
        const static int kind = 2;
        Expr * expr;
        Stmt * then;
        Stmt * els = nullptr;
        If(Expr * e, Stmt *then);
        If(Expr * e, Stmt *then, Stmt *els);
        int get_kind() {
            return kind;
        }
        void print() {
            printf("(if ");
            expr->print();
            printf(" then ");
            then->print();
            if (els) { printf("else "); els->print();}
            printf(") ");
        }
};

class While : public Stmt {
    public:
        const static int kind = 3;
        Expr * expr;
        Stmt * stmt;
        While(Expr * e, Stmt *s);
        int get_kind() {
            return kind;
        }
        void print() {
            printf("(while ");
            expr->print();
            stmt->print();
            printf(") ");
        }
};

class Return : public Stmt {
    public:
        const static int kind = 4;
        Expr * expr;
        Return(Expr * e);
        int get_kind() {
            return kind;
        }
        void print() {
            printf("(return ");
            expr->print();
            printf(") ");
        }
};

class Var : public Expr {
    public:
    const static int kind = 7;
    // Should be a pointer to the symbol table.
    int type;
    const char * name;
    Var(const char * name);
    void print()
    {
        printf("%s", name);
    }
    int get_kind() { return kind; }
};

class VarDecl : public Stmt {
    public:
    int type;
    Var * v;
    VarDecl(int, Var *);
    int get_kind() { return 0; }
    void print() {
        printf("var");
        putchar(' ');
        v->print();
        putchar(' ');
    }
};

class Function : public Stmt {
    public:
    const static int kind = 6;
    int returnType;
    Stmt *stmts;
    Stmt *params;
    Function(int, Stmt *, Stmt *);
    void print() {
        printf("(fun ");
        printf("(params ");
        if (params) {params->print();}
        putchar(')');
        printf(" (statements ");
        if (stmts) {stmts->print();}
        printf(") ");
    }
    int get_kind() { return kind; }

};

class String : public Expr {
    public:
        const static int kind = 5;
        const char * value;
        String(const char * value);
        int get_kind() {
            return kind;
        }
        void print() {
            printf("%s", value);
        }
};

class Int : public Expr {
    public:
        const static int kind = 6;
        const int value;
        Int(const int value);
        int get_kind() {
            return kind;
        }
        void print() {
            printf("%d", value);
        }
};

class Float: public Expr {
    public:
        const static int kind = 7;
        const float value;
        Float(const float value);
        int get_kind() {
            return kind;
        }
        void print() {
            printf("%f", value);
        }
};

class Paren: public Expr {
    public:
        const static int kind = 8;
        Expr * expr;
        Paren(Expr * e);
        int get_kind() {
            return kind;
        }
        void print() {
            putchar('(');
            expr->print();
            putchar(')');
        }
};

class BitwiseExpr: public Expr {
    public:
        const static int kind = 9;
        char * op;
        Expr * left;
        Expr * right;
        BitwiseExpr(char *, Expr *, Expr *);
        int get_kind() {
            return kind;
        }
        void print() {
            left->print();
            printf(%s, op);
            right->print();
        }
};

class UnaryExpr: public Expr {
    public:
        const static int kind = 10;
        char * op;
        Expr * expr;
        UnaryExpr(char *, Expr *);
        int get_kind() {
            return kind;
        }
        void print() {
            expr->print();
            printf(%s, op);
        }
};

class TernaryExpr: public Expr {
    public:
        const static int kind = 11;
        Expr * condition;
        Expr * t_expr;
        Expr * f_expr;
        TernaryExpr(Expr *, Expr *, Expr *);
        int get_kind() {
            return kind;
        }
        void print() {
            condition->print();
            putchar('?');
            t_expr->print();
            putchar(':');
            f_expr->print();
        }
};

class CompExpr : public Expr {
    public:
        const static int kind = 12;
        char * op;
        Expr * left;
        Expr * right;
        CompExpr(char *, Expr *, Expr *);
        int get_kind() {
            return kind;
        }
        void print() {
            left->print();
            printf(%s, op);
            right->print();
        }
};

class LogicalExpr : public Expr {
    public:
        const static int kind = 13;
        char * op;
        Expr * left;
        Expr * right;
        LogicalExpr(char *, Expr *, Expr *);
        int get_kind() {
            return kind;
        }
        void print() {
            left->print();
            printf(%s, op);
            right->print();
        }
};
#endif
