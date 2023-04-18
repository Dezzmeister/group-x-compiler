/**
 * This file contains classes representing crucial elements of the syntax. These include things
 * like int literals, variable declarations, type expressions, etc. Together the classes
 * in this file can be used to build an AST that represents a program. Every class
 * inherits from the abstract class ASTNode. The general model is that no class inherits
 * from a non-abstract class, and abstract classes are used to define useful abstractions
 * of different kinds of syntax elements. For example, the two abstract classes Expr and Statement
 * both inherit from ASTNode, but they represent different things and their derived
 * classes represent specific instances of Expr/Statement.
 *
 * The class hierarchy forms a tree where every leaf node is a non-abstract class, and every other
 * node is abstract. The leaf nodes all have a unique static member called 'kind' which is used at runtime
 * to narrow the abstract type. You can call get_kind() on the object in question and compare it to
 * Class::kind to see if you can make a narrowing cast. The KIND_CLASS macro copies some of the
 * boilerplate to add the static kind member and implement the abstract function. The kinds are
 * initialized in ast.cpp with names; you can use x::kind_map to look up the name for a given kind.
 *
 * Many of these node classes have a similar structure; e.g., 'left' and 'right' Expr nodes as 
 * members, or similar class members with different types in different classes. Usually we would
 * use generics (templates) to reduce boilerplate and simplify the class hierarchy but in this case we chose
 * not to because we wanted to restrict the set of allowable ASTNodes and keep the classes themselves
 * very simple. The class hierarchy is nice because most classes match up with a grammar production.
 *
 * ======= Memory Model =======
 *
 * Many of these classes accept pointers to nodes in their constructors. In general, the class takes
 * ownership of the node passed in and is responsible for destroying it. If you add a node that owns
 * other nodes, please implement the destructor.
 */
#ifndef SRC_AST_H
#define SRC_AST_H

#include <iostream>
#include <string>
#include <vector>

#include "symtable.h"

#define KIND_CLASS() \
    const static int kind; \
    \
    int get_kind() { \
        return this->kind; \
    }

class ProgramSource;

namespace x {
    extern std::vector<std::string> kind_map;

    int next_kind(const char * const name);

    void tree_dotfile(std::ostream &out, ProgramSource * prog);
}

class ASTNode {
    public:
        virtual int get_kind() = 0;

        virtual void print() const = 0;
        virtual std::vector<ASTNode *> children() = 0;

        virtual ~ASTNode() {}

    protected:
        ASTNode() {}
};

class ProgramSource : public ASTNode {
    public:
        std::vector<ASTNode *> nodes;

        ProgramSource(std::vector<ASTNode *> nodes);

        virtual ~ProgramSource();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        void add_node(ASTNode * node);

        KIND_CLASS()
};

class Expr : public ASTNode {
    public:
        virtual void print() const = 0;
        virtual std::vector<ASTNode *> children() = 0;

        virtual ~Expr() {}

    protected:
        Expr() {}
};

class ExprList : public ASTNode {
    public:
        std::vector<Expr *> exprs;

        ExprList(std::vector<Expr *> exprs);

        virtual ~ExprList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        void push_expr(Expr * expr);

        KIND_CLASS()
};

class CallingExpr : public Expr {
    public:
        virtual void print() const = 0;
        virtual std::vector<ASTNode *> children() = 0;

        virtual ~CallingExpr() {}

    protected:
        CallingExpr() {}
};

class Statement : public ASTNode {
    public:
        virtual void print() const = 0;
        virtual std::vector<ASTNode *> children() = 0;

        virtual ~Statement() {}

    protected:
        Statement() {}
};

class StatementList : public ASTNode {
    public:
        std::vector<Statement *> statements;

        StatementList(std::vector<Statement *> statements);

        virtual ~StatementList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        void push_statement(Statement * statement);

        KIND_CLASS()
};

class ParensExpr : public CallingExpr {
    public:
        const Expr * expr;

        ParensExpr(const Expr * expr);

        virtual ~ParensExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class TypeDecl : public Statement {
    public:
        virtual void print() const = 0;
        virtual std::vector<ASTNode *> children() = 0;

        virtual ~TypeDecl() {}

    protected:
        TypeDecl() {}
};

class Typename : public ASTNode {
    public:
        virtual void print() const = 0;
        virtual std::vector<ASTNode *> children() = 0;

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
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class NumLiteral : public Expr {
    public:
        virtual void print() const = 0;
        virtual std::vector<ASTNode *> children() = 0;

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
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class FloatLiteral : public NumLiteral {
    public:        
        const double value;

        FloatLiteral(const char * float_str);
        FloatLiteral(const float value);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class TernaryExpr: public Expr {
    public:
        const Expr * cond;
        const Expr * tru;
        const Expr * fals;

        TernaryExpr(const Expr * cond, const Expr * tru, const Expr * fals);

        virtual ~TernaryExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class BoolLiteral : public Expr {
    public:
        const bool value;

        BoolLiteral(const bool value);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class TypeIdent : public Typename {
    public:
        const std::string id;

        TypeIdent(const char * const _id);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class Ident : public CallingExpr {
    public:
        const std::string id;

        Ident(const char * const _id);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class MathExpr : public Expr {
    public:
        const char op;
        const Expr * left;
        const Expr * right;

        MathExpr(const char op, const Expr * left, const Expr * right);

        virtual ~MathExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class BoolExpr : public Expr {
    public:
        const std::string op;
        const Expr * left;
        const Expr * right;

        BoolExpr(const char * const op, const Expr * left, const Expr * right);

        virtual ~BoolExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class PtrTypename : public Typename {
    public:
        const Typename * name;

        PtrTypename(const Typename * name);

        virtual ~PtrTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS() 
};

class MutTypename : public Typename {
    public:
        const Typename * name;

        MutTypename(const Typename * name);

        virtual ~MutTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class TypenameList : public ASTNode {
    public:
        std::vector<Typename *> types;

        TypenameList(std::vector<Typename *> types);

        virtual ~TypenameList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

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
        virtual std::vector<ASTNode *> children();

        void add_to_scope(SymbolTable * symtable);

        KIND_CLASS()
};

class ParamsList : public ASTNode {
    public:
        std::vector<VarDecl *> params;

        ParamsList(std::vector<VarDecl *> params);

        virtual ~ParamsList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        void push_param(VarDecl * param);

        void add_to_scope(SymbolTable * symtable);

        KIND_CLASS()
};

// TODO: Find a way to avoid diamond inheritance here
class FunctionCall : public Statement, public Expr {
    public:
        const CallingExpr * func;
        const ExprList * args;

        FunctionCall(const CallingExpr * func, const ExprList * args);

        virtual ~FunctionCall();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class VarDeclList : public ASTNode {
    public:
        std::vector<VarDecl *> decls;

        VarDeclList(std::vector<VarDecl *> decls);

        virtual ~VarDeclList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

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
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class TupleExpr : public Expr {
    public:
        const ExprList * expr_list;

        TupleExpr(const ExprList * expr_list);

        virtual ~TupleExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class FuncTypename : public Typename {
    public:
        const TypenameList * params;
        const Typename * ret_type;

        FuncTypename(const TypenameList * params, const Typename * ret_type);

        virtual ~FuncTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class StaticArrayTypename : public Typename {
    public:
        const Typename * element_type;
        const IntLiteral * size;

        StaticArrayTypename(const Typename * element_type, const IntLiteral * size);

        virtual ~StaticArrayTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class TypeAlias : public TypeDecl {
    public:
        const Ident * name;
        const Typename * type_expr;

        TypeAlias(const Ident * name, const Typename * type_expr);

        virtual ~TypeAlias();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class StructDecl : public TypeDecl {
    public:
        const Ident * name;
        const VarDeclList * members;

        StructDecl(const Ident * name, const VarDeclList * members);

        virtual ~StructDecl();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class VarDeclInit : public Statement {
    public:
        const VarDecl * decl;
        const Expr * init;

        VarDeclInit(const VarDecl * decl, const Expr * init);

        virtual ~VarDeclInit();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class ArrayLiteral : public Expr {
    public:
        const ExprList * items;

        ArrayLiteral(const ExprList * items);

        virtual ~ArrayLiteral();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class IfStmt : public Statement {
    public:
        const Expr * cond;
        const StatementList * then;

        IfStmt(const Expr * cond, const StatementList * then);

        virtual ~IfStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class IfElseStmt : public Statement {
    public:
        const IfStmt * if_stmt;
        const StatementList * els;

        IfElseStmt(const IfStmt * if_stmt, const StatementList * els);

        virtual ~IfElseStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class WhileStmt : public Statement {
    public:
        const Expr * cond;
        const StatementList * body;

        WhileStmt(const Expr * cond, const StatementList * body);

        virtual ~WhileStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class ForStmt : public Statement {
    public:
        const Expr * init, *condition, *update;
        const StatementList * body;

        ForStmt(const Expr *init, const Expr *condition, 
        const Expr *update, const StatementList * body);

        virtual ~ForStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class AddrOf : public Expr {
    public:
        const Expr * expr;

        AddrOf(const Expr * expr);

        virtual ~AddrOf();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class Deref : public Expr {
    public:
        const Expr * expr;

        Deref(const Expr * expr);

        virtual ~Deref();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class CastExpr : public Expr {
    public:
        const Typename * dest_type;
        const Expr * expr;

        CastExpr(const Typename * dest_type, const Expr * expr);

        virtual ~CastExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class LogicalExpr : public Expr {
    public:
        const std::string op;
        const Expr * left;
        const Expr * right;

        LogicalExpr(std::string op , const Expr *l, const Expr *r);

        virtual ~LogicalExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class FuncDecl : public ASTNode {
    public:
        const Ident * name;
        const ParamsList * params;
        const Typename * ret_type;
        const StatementList * body;

        FuncDecl(const Ident * name, const ParamsList * params, const Typename * ret_body, const StatementList * body);

        virtual ~FuncDecl();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class ReturnStatement : public Statement {
    public:
        const Expr * val;

        ReturnStatement(const Expr * val);

        virtual ~ReturnStatement();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

class Assignment : public Statement {
    public:
        // TODO (maybe): Distinguish between lvalue and rvalue at parser level?
        const Expr * lhs;
        const Expr * rhs;

        Assignment(const Expr * lhs, const Expr * rhs);

        virtual ~Assignment();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        KIND_CLASS()
};

#endif
