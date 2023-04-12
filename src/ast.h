#ifndef SRC_AST_H
#define SRC_AST_H

#include <string>
#include <vector>

#define KIND_CLASS() \
    const static int kind; \
    \
    int get_kind() { \
        return this->kind; \
    }

namespace x {
    extern std::vector<std::string> kind_map;

    int next_kind(const char * const name);
}

class ASTNode {
    public:
        virtual int get_kind() = 0;

        virtual void print() const = 0;

        virtual ~ASTNode() {}

    protected:
        ASTNode() {}
};

class Expr : public ASTNode {
    public:
        virtual void print() const = 0;

        virtual ~Expr() {}

    protected:
        Expr() {}
};

class Statement : public ASTNode {
    public:
        virtual void print() const = 0;

        virtual ~Statement() {}

    protected:
        Statement() {}
};

class TypeDecl : public Statement {
    public:
        virtual void print() const = 0;

        virtual ~TypeDecl() {}

    protected:
        TypeDecl() {}
};

class Typename : public ASTNode {
    public:
        virtual void print() const = 0;

        virtual ~Typename() {}

    protected:
        Typename() {}
};

class ParensTypename : public Typename {
    public:
        const Typename * name;

        ParensTypename(const Typename * name);

        virtual ~ParensTypename();

        virtual void print() const;

        KIND_CLASS()
};

class MathExpr : public ASTNode {
    public:
        virtual void print() const = 0;

        virtual ~MathExpr() {}

    protected:
        MathExpr() {}
};

class NumLiteral : public MathExpr {
    public:
        virtual void print() const = 0;

        virtual ~NumLiteral() {}

    protected:
        NumLiteral() {}
};

class IntLiteral : public NumLiteral {
    public:
        const int value;
        
        IntLiteral(const char * int_str);

        IntLiteral(const int value);

        virtual void print() const;

        KIND_CLASS()
};

class FloatLiteral : public NumLiteral {
    public:        
        const double value;

        FloatLiteral(const char * float_str);

        FloatLiteral(const float value);

        virtual void print() const;

        KIND_CLASS()
};

class Ident : public Typename, Expr {
    public:
        const std::string id;

        Ident(const char * const _id);

        virtual void print() const;

        KIND_CLASS()
};

class MathOp : public MathExpr {
    public:
        const char op;

        std::vector<MathExpr *> operands;

        MathOp(const char op, std::vector<MathExpr *> operands);

        virtual ~MathOp();

        virtual void print() const;

        // Pushes the operand onto the back of the operand list
        void push_operand(MathExpr * operand);

        KIND_CLASS()
};

class PtrTypename : public Typename {
    public:
        const Typename * name;

        PtrTypename(const Typename * name);

        virtual ~PtrTypename();

        virtual void print() const;

        KIND_CLASS() 
};

class MutTypename : public Typename {
    public:
        const Typename * name;

        MutTypename(const Typename * name);

        virtual ~MutTypename();

        virtual void print() const;

        KIND_CLASS()
};

class TypenameList : public ASTNode {
    public:
        std::vector<Typename *> types;

        TypenameList(std::vector<Typename *> types);

        virtual ~TypenameList();

        virtual void print() const;

        // Pushes the typename onto the back of the type list
        void push_type(Typename * type_name);

        KIND_CLASS()
};

class VarDecl : public Statement {
    public:
        const Typename * type_name;
        const Ident * var_name;

        VarDecl(const Typename * type_name, const Ident * var_name);

        virtual ~VarDecl();

        virtual void print() const;

        KIND_CLASS()
};

class VarDeclList : public ASTNode {
    public:
        std::vector<VarDecl *> decls;

        VarDeclList(std::vector<VarDecl *> decls);

        virtual ~VarDeclList();

        virtual void print() const;

        // Pushes the var decl onto the back of the type list
        void push_decl(VarDecl * decl);

        KIND_CLASS()
};

class TupleTypename : public Typename {
    public:
        const TypenameList * type_list;

        TupleTypename(const TypenameList * type_list);

        virtual ~TupleTypename();

        virtual void print() const;

        KIND_CLASS()
};

class FuncTypename : public Typename {
    public:
        const TypenameList * params;
        const Typename * ret_type;

        FuncTypename(const TypenameList * params, const Typename * ret_type);

        virtual ~FuncTypename();

        virtual void print() const;

        KIND_CLASS()
};

class TypeAlias : public TypeDecl {
    public:
        const Ident * name;
        const Typename * type_expr;

        TypeAlias(const Ident * name, const Typename * type_expr);

        virtual ~TypeAlias();

        virtual void print() const;

        KIND_CLASS()
};

class StructDecl : public TypeDecl {
    public:
        const Ident * name;
        const VarDeclList * members;

        StructDecl(const Ident * name, const VarDeclList * members);

        virtual ~StructDecl();

        virtual void print() const;

        KIND_CLASS()
};

#endif
