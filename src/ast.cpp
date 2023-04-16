#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "ast.h"

std::vector<std::string> x::kind_map;

const int ProgramSource::kind = x::next_kind("program_source");
const int IntLiteral::kind = x::next_kind("int_literal");
const int FloatLiteral::kind = x::next_kind("float_literal");
const int BoolLiteral::kind = x::next_kind("bool_literal");
const int Ident::kind = x::next_kind("ident");
const int ParensExpr::kind = x::next_kind("parens_expr");
const int MathExpr::kind = x::next_kind("math_expr");
const int BoolExpr::kind = x::next_kind("bool_expr");
const int ParensTypename::kind = x::next_kind("parens_typename");
const int PtrTypename::kind = x::next_kind("ptr_typename");
const int MutTypename::kind = x::next_kind("mut_typename");
const int TypenameList::kind = x::next_kind("typename_list");
const int VarDeclList::kind = x::next_kind("var_decl_list");
const int ExprList::kind = x::next_kind("expr_list");
const int TupleTypename::kind = x::next_kind("tuple_typename");
const int FuncTypename::kind = x::next_kind("func_typename");
const int TypeAlias::kind = x::next_kind("type_alias");
const int StructDecl::kind = x::next_kind("struct_decl");
const int VarDecl::kind = x::next_kind("var_decl");
const int VarDeclInit::kind = x::next_kind("var_decl_init");
const int AddrOf::kind = x::next_kind("addr_of");
const int Deref::kind = x::next_kind("deref");
const int CastExpr::kind = x::next_kind("cast_expr");
const int TupleExpr::kind = x::next_kind("tuple_expr");
const int FunctionCall::kind = x::next_kind("function_call");
const int StatementList::kind = x::next_kind("statement_list");
const int ArgsList::kind = x::next_kind("args_list");
const int FuncDecl::kind = x::next_kind("func_decl");
const int ReturnStatement::kind = x::next_kind("return_statement");

int x::next_kind(const char * const name) {
    static int kind = 0;
    x::kind_map.push_back(std::string(name));

    return kind++;
}

IntLiteral::IntLiteral(const char * int_str) : value(atoi(int_str)) {}

IntLiteral::IntLiteral(const int value) : value(value) {}

void IntLiteral::print() const {
    printf("%d", value);
}

FloatLiteral::FloatLiteral(const char * float_str) : value(atof(float_str)) {}

FloatLiteral::FloatLiteral(const float value) : value(value) {}

void FloatLiteral::print() const {
    printf("%f", value);
}

BoolLiteral::BoolLiteral(const bool value) : value(value) {}

void BoolLiteral::print() const {
    if (value) {
        printf("true");
    } else {
        printf("false");
    }
}

Ident::Ident(const char * const _id) : id(std::string(_id)) {}

void Ident::print() const {
    std::cout << id;
}

MathExpr::MathExpr(const char op, const Expr * left, const Expr * right) :
    op(op),
    left(left),
    right(right)
    {}

MathExpr::~MathExpr() {
    delete left;
    delete right;
}

void MathExpr::print() const {
    left->print();
    printf(" %c ", op);
    right->print();
}

BoolExpr::BoolExpr(const char * const op, const Expr * left, const Expr * right) :
    op(std::string(op)),
    left(left),
    right(right)
    {}

BoolExpr::~BoolExpr() {
    delete left;
    delete right;
}

void BoolExpr::print() const {
    left->print();
    printf(" %s ", op.c_str());
    right->print();
}

ParensExpr::ParensExpr(const Expr * expr) : expr(expr) {}

ParensExpr::~ParensExpr() {
    delete expr;
}

void ParensExpr::print() const {
    putchar('(');
    expr->print();
    putchar(')');
}

ParensTypename::ParensTypename(const Typename * name) : name(name) {}

ParensTypename::~ParensTypename() {
    delete name;
}

void ParensTypename::print() const {
    putchar('(');
    name->print();
    putchar(')');
}

PtrTypename::PtrTypename(const Typename * name) : name(name) {}

PtrTypename::~PtrTypename() {
    delete name;
}

void PtrTypename::print() const {
    name->print();
    putchar('*');
}

MutTypename::MutTypename(const Typename * name) : name(name) {}

MutTypename::~MutTypename() {
    delete name;
}

void MutTypename::print() const {
    printf("mut ");
    name->print();
}

TypenameList::TypenameList(std::vector<Typename *> types) : types(types) {}

TypenameList::~TypenameList() {
    for (auto &type_name : types) {
        delete type_name;
    }
}

void TypenameList::print() const {
    for (size_t i = 0; i < types.size() - 1; i++) {
        types[i]->print();
        printf(", ");
    }

    types[types.size() - 1]->print();
}

void TypenameList::push_type(Typename * type_name) {
    types.push_back(type_name);
}

VarDeclList::VarDeclList(std::vector<VarDecl *> decls) : decls(decls) {}

VarDeclList::~VarDeclList() {
    for (auto &decl : decls) {
        delete decl;
    }
}

void VarDeclList::print() const {
    for (auto &decl : decls) {
        decl->print();
        printf(";\n");
    }
}

void VarDeclList::push_decl(VarDecl * decl) {
    decls.push_back(decl);
}

ExprList::ExprList(std::vector<Expr *> exprs) : exprs(exprs) {}

ExprList::~ExprList() {
    for (auto &expr : exprs) {
        delete expr;
    }
}

void ExprList::print() const {
    for (size_t i = 0; i < exprs.size() - 1; i++) {
        exprs[i]->print();
        printf(", ");
    }

    exprs[exprs.size() - 1]->print();
}

void ExprList::push_expr(Expr * expr) {
    exprs.push_back(expr);
}

StatementList::StatementList(std::vector<Statement *> statements) : statements(statements) {}

StatementList::~StatementList() {
    for (auto &statement : statements) {
        delete statement;
    }
}

void StatementList::print() const {
    for (auto &statement : statements) {
        statement->print();
        printf(";\n");
    }
}

void StatementList::push_statement(Statement * statement) {
    statements.push_back(statement);
}

TupleTypename::TupleTypename(const TypenameList * type_list) : type_list(type_list) {}

TupleTypename::~TupleTypename() {
    delete type_list;
}

void TupleTypename::print() const {
    putchar('[');
    type_list->print();
    putchar(']');
}

TupleExpr::TupleExpr(const ExprList * expr_list) : expr_list(expr_list) {}

TupleExpr::~TupleExpr() {
    delete expr_list;
}

void TupleExpr::print() const {
    putchar('[');
    expr_list->print();
    putchar(']');
}

FuncTypename::FuncTypename(const TypenameList * params, const Typename * ret_type) :
    params(params),
    ret_type(ret_type)
    {}

FuncTypename::~FuncTypename() {
    delete params;
    delete ret_type;
}

void FuncTypename::print() const {
    putchar('[');
    params->print();
    printf("] -> ");
    ret_type->print();
}

TypeAlias::TypeAlias(const Ident * name, const Typename * type_expr) : name(name), type_expr(type_expr) {}

TypeAlias::~TypeAlias() {
    delete name;
    delete type_expr;
}

void TypeAlias::print() const {
    printf("type ");
    name->print();
    printf(" = ");
    type_expr->print();
}

StructDecl::StructDecl(const Ident * name, const VarDeclList * members) : name(name), members(members) {}

StructDecl::~StructDecl() {
    delete name;
    delete members;
}

void StructDecl::print() const {
    printf("struct ");
    name->print();
    printf(" {\n");
    members->print();
    printf("};\n");
}

VarDecl::VarDecl(const Typename * type_name, const Ident * var_name) :
    type_name(type_name),
    var_name(var_name)
    {}

VarDecl::~VarDecl() {
    delete type_name;
    delete var_name;
}

void VarDecl::print() const {
    type_name->print();
    putchar(' ');
    var_name->print();
}

void VarDecl::add_to_scope(SymbolTable * symtable) {
    symtable->put(var_name->id, new Symbol(Var));
}

VarDeclInit::VarDeclInit(const VarDecl * decl, const Expr * init) : decl(decl), init(init) {}

VarDeclInit::~VarDeclInit() {
    delete decl;
    delete init;
}

void VarDeclInit::print() const {
    decl->print();
    printf(" = ");
    init->print();
}

AddrOf::AddrOf(const Expr * expr) : expr(expr) {}

AddrOf::~AddrOf() {
    delete expr;
}

void AddrOf::print() const {
    putchar('&');
    expr->print();
}

Deref::Deref(const Expr * expr) : expr(expr) {}

Deref::~Deref() {
    delete expr;
}

void Deref::print() const {
    putchar('*');
    expr->print();
}

CastExpr::CastExpr(const Typename * dest_type, const Expr * expr) : dest_type(dest_type), expr(expr) {}

CastExpr::~CastExpr() {
    delete dest_type;
    delete expr;
}

void CastExpr::print() const {
    expr->print();
    printf(" as ");
    dest_type->print();
}

FunctionCall::FunctionCall(const CallingExpr * func, const ExprList * args) :
    func(func),
    args(args)
    {}

FunctionCall::~FunctionCall() {
    delete func;
    delete args;
}

void FunctionCall::print() const {
    func->print();
    putchar('(');
    args->print();
    putchar(')');
}

ArgsList::ArgsList(std::vector<VarDecl *> args) : args(args) {}

ArgsList::~ArgsList() {
    for (auto &arg : args) {
        delete arg;
    }
}

void ArgsList::print() const {
    for (size_t i = 0; i < args.size() - 1; i++) {
        args[i]->print();
        printf(", ");
    }

    args[args.size() - 1]->print();
}

void ArgsList::push_arg(VarDecl * arg) {
    args.push_back(arg);
}

void ArgsList::add_to_scope(SymbolTable * symtable) {
    for (auto &arg : args) {
        arg->add_to_scope(symtable);
    }
}

FuncDecl::FuncDecl(const Ident * name, const ArgsList * params, const Typename * ret_type, const StatementList * body) :
    name(name),
    params(params),
    ret_type(ret_type),
    body(body)
    {}

FuncDecl::~FuncDecl() {
    delete name;
    delete params;
    delete ret_type;
    delete body;
}

void FuncDecl::print() const {
    ret_type->print();
    putchar(' ');
    name->print();
    putchar('(');
    params->print();
    printf(") {\n");
    body->print();
    printf("}\n");
}

ProgramSource::ProgramSource(std::vector<ASTNode *> nodes) : nodes(nodes) {}

ProgramSource::~ProgramSource() {
    nodes.clear();
}

void ProgramSource::print() const {
    for (auto &node : nodes) {
        node->print();
        printf(";\n");
    }
}

void ProgramSource::add_node(ASTNode * node) {
    nodes.push_back(node);
}

ReturnStatement::ReturnStatement(const Expr * val) : val(val) {}

ReturnStatement::~ReturnStatement() {
    delete val;
}

void ReturnStatement::print() const {
    printf("return ");
    val->print();
}
