/**
 * This file contains classes representing crucial elements of the syntax. These
 * include things like int literals, variable declarations, type expressions,
 * etc. Together the classes in this file can be used to build an AST that
 * represents a program. Every class inherits from the abstract class ASTNode.
 * The general model is that no class inherits from a non-abstract class, and
 * abstract classes are used to define useful abstractions of different kinds of
 * syntax elements. For example, the two abstract classes Expr and Statement
 * both inherit from ASTNode, but they represent different things and their
 * derived classes represent specific instances of Expr/Statement.
 *
 * The class hierarchy forms a tree where every leaf node is a non-abstract
 * class, and every other node is abstract. The leaf nodes all have a unique
 * static member called 'kind' which is used at runtime to narrow the abstract
 * type. You can call get_kind() on the object in question and compare it to
 * Class::kind to see if you can make a narrowing cast. The KIND_CLASS macro
 * copies some of the boilerplate to add the static kind member and implement
 * the abstract function. The kinds are initialized in ast.cpp with names; you
 * can use x::kind_map to look up the name for a given kind.
 *
 * Many of these node classes have a similar structure; e.g., 'left' and 'right'
 * Expr nodes as members, or similar class members with different types in
 * different classes. Usually we would use generics (templates) to reduce
 * boilerplate and simplify the class hierarchy but in this case we chose not to
 * because we wanted to restrict the set of allowable ASTNodes and keep the
 * classes themselves very simple. The class hierarchy is nice because most
 * classes match up with a grammar production.
 *
 * ======= Memory Model =======
 *
 * Many of these classes accept pointers to nodes in their constructors. In
 * general, the class takes ownership of the node passed in and is responsible
 * for destroying it. If you add a node that owns other nodes, please implement
 * the destructor.
 */
#ifndef SRC_AST_H
#define SRC_AST_H

#include <iostream>
#include <string>
#include <vector>

#include "symtable.h"

#define KIND_CLASS()     \
  const static int kind; \
                         \
  virtual int get_kind() const { return this->kind; }

#define NEQ_OPERATOR()  \
  virtual bool operator!=(const ASTNode &node) const {  \
    return !(*this == node);  \
  }

struct YYLTYPE;
struct SourceErrors;

struct Location {
    int first_line;
    int first_col;
    int last_line;
    int last_col;

    Location(int first_line, int first_col, int last_line, int last_col);
    Location(YYLTYPE &start, YYLTYPE &end);
    Location(const Location &start, const Location &end);

    void set_end(YYLTYPE &end);
};

typedef struct Location Location;

class ProgramSource;
class ASTNode;
class Typename;

typedef bool (*FindFunc)(const ASTNode *);

namespace x {
    const Location NULL_LOC = Location(0, 0, 0, 0);

    extern std::vector<std::string> kind_map;

    int next_kind(const char * const name);

    void tree_dotfile(std::ostream &out, ProgramSource * prog);
}  // namespace x

class ASTNode {
    public:
        Location loc;

        virtual int get_kind() const = 0;

        virtual void print() const = 0;
        virtual std::vector<ASTNode *> children() = 0;
        virtual void gen() {}
        virtual ASTNode * find(FindFunc cond);

        virtual bool operator==(const ASTNode &node) const = 0;
        virtual bool operator!=(const ASTNode &node) const = 0;

        virtual ~ASTNode() {}

    protected:
        ASTNode(const Location loc) : loc(loc) {}
};

class ProgramSource : public ASTNode {
    public:
        std::string name;
        std::vector<ASTNode *> nodes;

        ProgramSource(const Location loc, std::string name, std::vector<ASTNode *> nodes);

        virtual ~ProgramSource();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        void add_node(ASTNode * node);

        KIND_CLASS()
};

class Expr : public ASTNode {
    public:
        virtual ~Expr() {}

        virtual Typename * type_of(SymbolTable * symtable) const = 0;

    protected:
        Expr(const Location loc) : ASTNode(loc) {}
};

class ExprList : public ASTNode {
    public:
        std::vector<Expr *> exprs;

        ExprList(const Location loc, std::vector<Expr *> exprs);

        virtual ~ExprList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        void push_expr(Expr * expr);

        KIND_CLASS()
};

class CallingExpr : public Expr {
    public:
        virtual ~CallingExpr() {}

    protected:
        CallingExpr(const Location loc) : Expr(loc) {}
};

class Statement : public ASTNode {
    public:
        virtual ~Statement() {}

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const = 0;

    protected:
        Statement(const Location loc) : ASTNode(loc) {}
};

class StatementList : public ASTNode {
    public:
        std::vector<Statement *> statements;

        StatementList(const Location loc, std::vector<Statement *> statements);

        virtual ~StatementList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        void push_statement(Statement * statement);

        KIND_CLASS()
};

class ParensExpr : public CallingExpr {
    public:
        const Expr * expr;

        ParensExpr(const Location loc, const Expr * expr);

        virtual ~ParensExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class TypeDecl : public Statement {
    public:
        virtual ~TypeDecl() {}

    protected:
        TypeDecl(const Location loc) : Statement(loc) {}
};

class Typename : public ASTNode {
    public:
        virtual ~Typename() {}

        virtual Typename * clone() const = 0;

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const = 0;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const = 0;

    protected:
        Typename(const Location loc) : ASTNode(loc) {}
};

class ParensTypename : public Typename {
    public:
        const Typename * name;

        ParensTypename(const Location loc, const Typename * name);

        virtual Typename * clone() const;

        virtual ~ParensTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class NumLiteral : public Expr {
    public:
        virtual ~NumLiteral() {}

    protected:
        NumLiteral(const Location loc) : Expr(loc) {}
};

class IntLiteral : public NumLiteral {
    public:
        const int value;

        IntLiteral(const Location loc, const char * int_str);

        IntLiteral(const Location loc, const int value);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class FloatLiteral : public NumLiteral {
    public:
        const double value;

        FloatLiteral(const Location loc, const char * float_str);
        FloatLiteral(const Location loc, const float value);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class TernaryExpr : public Expr {
    public:
        const Expr * cond;
        const Expr * tru;
        const Expr * fals;

        TernaryExpr(const Location loc, const Expr * cond, const Expr * tru, const Expr * fals);

        virtual ~TernaryExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class BoolLiteral : public Expr {
    public:
        const bool value;

        BoolLiteral(const Location loc, const bool value);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class CharLiteral : public Expr {
    public:
        const char value;

        CharLiteral(const Location loc, const char value);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class StringLiteral : public Expr {
    public:
        const std::string value;

        StringLiteral(const Location loc, const char * const value);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class TypeIdent : public Typename {
    public:
        const std::string id;

        TypeIdent(const Location loc, const char * const _id);

        virtual Typename * clone() const;

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class Ident : public CallingExpr {
    public:
        const std::string id;

        Ident(const Location loc, const char * const _id);

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class MathExpr : public Expr {
    public:
        const char op;
        const Expr * left;
        const Expr * right;

        MathExpr(const Location loc, const char op, const Expr * left, const Expr * right);

        virtual ~MathExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class BoolExpr : public Expr {
    public:
        const std::string op;
        const Expr * left;
        const Expr * right;

        BoolExpr(const Location loc, const char * const op, const Expr * left, const Expr * right);

        virtual ~BoolExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class PtrTypename : public Typename {
    public:
        const Typename * name;

        PtrTypename(const Location loc, const Typename * name);

        virtual Typename * clone() const;

        virtual ~PtrTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class MutTypename : public Typename {
    public:
        const Typename * name;

        MutTypename(const Location loc, const Typename * name);

        virtual Typename * clone() const;

        virtual ~MutTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class TypenameList : public ASTNode {
    public:
        std::vector<Typename *> types;

        TypenameList(const Location loc, std::vector<Typename *> types);

        virtual ~TypenameList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        // Pushes the typename onto the back of the type list
        void push_type(Typename * type_name);

        KIND_CLASS()
};

class VarDecl : public Statement {
    public:
        const Typename * type_name;
        const Ident * var_name;

        VarDecl(const Location loc, const Typename * type_name, const Ident * var_name);

        virtual ~VarDecl();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();
        
        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        void add_to_scope(SymbolTable * symtable);

        KIND_CLASS()
};

class ParamsList : public ASTNode {
    public:
        std::vector<VarDecl *> params;

        ParamsList(const Location loc, std::vector<VarDecl *> params);

        virtual ~ParamsList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        void push_param(VarDecl * param);

        void add_to_scope(SymbolTable * symtable);

        KIND_CLASS()
};

// TODO: Inherit from CallingExpr instead
class FunctionCallExpr : public Expr {
    public:
        const CallingExpr * func;
        const ExprList * args;

        FunctionCallExpr(const Location loc, const CallingExpr * func, const ExprList * args);

        virtual ~FunctionCallExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

// Identical to FunctionCallExpr, except it inherits from Stmt instead. This is
// to avoid diamond inheritance which causes major problems especially when
// downcasting. Having two nearly identical classes is a lesser evil
class FunctionCallStmt : public Statement {
    public:
        const CallingExpr * func;
        const ExprList * args;

        FunctionCallStmt(const Location loc, const CallingExpr * func, const ExprList * args);

        virtual ~FunctionCallStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class VarDeclList : public ASTNode {
    public:
        std::vector<VarDecl *> decls;

        VarDeclList(const Location loc, std::vector<VarDecl *> decls);

        virtual ~VarDeclList();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        // Pushes the var decl onto the back of the type list
        void push_decl(VarDecl * decl);

        KIND_CLASS()
};

class TupleTypename : public Typename {
    public:
        const TypenameList * type_list;

        TupleTypename(const Location loc, const TypenameList * type_list);

        virtual Typename * clone() const;

        virtual ~TupleTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class TupleExpr : public Expr {
    public:
        const ExprList * expr_list;

        TupleExpr(const Location loc, const ExprList * expr_list);

        virtual ~TupleExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class FuncTypename : public Typename {
    public:
        const TypenameList * params;
        const Typename * ret_type;

        FuncTypename(const Location loc, const TypenameList * params, const Typename * ret_type);

        virtual Typename * clone() const;

        virtual ~FuncTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class StaticArrayTypename : public Typename {
    public:
        const Typename * element_type;
        const IntLiteral * size;

        StaticArrayTypename(const Location loc, const Typename * element_type, const IntLiteral * size);

        virtual Typename * clone() const;

        virtual ~StaticArrayTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class TypeAlias : public TypeDecl {
    public:
        const Ident * name;
        const Typename * type_expr;

        TypeAlias(const Location loc, const Ident * name, const Typename * type_expr);

        virtual ~TypeAlias();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

/**
 * NB: The struct typename does not own the symbol table and will not delete it upon
 * destruction.
 */
class StructTypename : public Typename {
    public:
        const VarDeclList * members;
        const SymbolTable * scope;

        StructTypename(const Location loc, const VarDeclList * members, const SymbolTable * scope);

        virtual Typename * clone() const;

        virtual ~StructTypename();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual bool type_equals(const Typename * t, SymbolTable * symtable) const;
        virtual bool can_cast_to(const Typename * t, SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class StructDecl : public TypeDecl {
    public:
        const Ident * name;
        const StructTypename * defn;

        StructDecl(const Location loc, const Ident * name, const StructTypename * defn);

        virtual ~StructDecl();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class VarDeclInit : public Statement {
    public:
        const VarDecl * decl;
        const Expr * init;
        int type;

        VarDeclInit(const Location loc, const VarDecl * decl, const Expr * init);

        virtual ~VarDeclInit();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class ArrayLiteral : public Expr {
    public:
        const ExprList * items;

        ArrayLiteral(const Location loc, const ExprList * items);

        virtual ~ArrayLiteral();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class IfStmt : public Statement {
    public:
        const Expr * cond;
        const StatementList * then;
        SymbolTable * scope;

        IfStmt(const Location loc, const Expr * cond, const StatementList * then, SymbolTable * scope);

        virtual ~IfStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class IfElseStmt : public Statement {
    public:
        const IfStmt * if_stmt;
        const StatementList * els;
        SymbolTable * scope;

        IfElseStmt(const Location loc, const IfStmt * if_stmt, const StatementList * els, SymbolTable * scope);

        virtual ~IfElseStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class WhileStmt : public Statement {
    public:
        const Expr * cond;
        const StatementList * body;
        SymbolTable * scope;

        WhileStmt(const Location loc, const Expr * cond, const StatementList * body, SymbolTable * scope);

        virtual ~WhileStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class ForStmt : public Statement {
    public:
        const Statement * init;
        const Expr * condition;
        const Statement * update;
        const StatementList * body;
        SymbolTable * scope;

        ForStmt(const Location loc, const Statement * init, const Expr * condition, const Statement * update,
                const StatementList * body, SymbolTable * scope);

        virtual ~ForStmt();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class AddrOf : public Expr {
    public:
        const Expr * expr;

        AddrOf(const Location loc, const Expr * expr);

        virtual ~AddrOf();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class Deref : public Expr {
    public:
        const Expr * expr;

        Deref(const Location loc, const Expr * expr);

        virtual ~Deref();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class CastExpr : public Expr {
    public:
        const Typename * dest_type;
        const Expr * expr;

        CastExpr(const Location loc, const Typename * dest_type, const Expr * expr);

        virtual ~CastExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class LogicalExpr : public Expr {
    public:
        const std::string op;
        const Expr * left;
        const Expr * right;

        LogicalExpr(const Location loc, const char * const op, const Expr * l, const Expr * r);

        virtual ~LogicalExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class FuncDecl : public ASTNode {
    public:
        const Ident * name;
        const ParamsList * params;
        const Typename * ret_type;
        const StatementList * body;
        SymbolTable * scope;

        FuncDecl(const Location loc, const Ident * name, const ParamsList * params,
                 const Typename * ret_body, const StatementList * body, SymbolTable * scope);

        virtual ~FuncDecl();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class ReturnStatement : public Statement {
    public:
        const Expr * val;

        ReturnStatement(const Location loc, const Expr * val);

        virtual ~ReturnStatement();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class Assignment : public Statement {
    public:
        // TODO (maybe): Distinguish between lvalue and rvalue at parser level?
        const Expr * lhs;
        const Expr * rhs;

        Assignment(const Location loc, const Expr * lhs, const Expr * rhs);

        virtual ~Assignment();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual void typecheck(SymbolTable * symtable, SourceErrors &errors) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class BangExpr : public Expr {
    public:
        const Expr * expr;

        BangExpr(const Location loc, const Expr * expr);

        virtual ~BangExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class NotExpr : public Expr {
    public:
        const Expr * expr;

        NotExpr(const Location loc, const Expr * expr);

        virtual ~NotExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

class PreExpr : public Expr {
    public:
        const std::string op;
        const Expr * expr;

        PreExpr(const Location loc, const char * const op, const Expr * expr);

        virtual ~PreExpr();

        virtual void print() const;
        virtual std::vector<ASTNode *> children();

        virtual Typename * type_of(SymbolTable * symtable) const;

        virtual bool operator==(const ASTNode &node) const;
        NEQ_OPERATOR()

        KIND_CLASS()
};

#endif
