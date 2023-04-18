#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <string.h>

#include "ast.h"

std::vector<std::string> x::kind_map;

const int ProgramSource::kind = x::next_kind("program_source");
const int IntLiteral::kind = x::next_kind("int_literal");
const int FloatLiteral::kind = x::next_kind("float_literal");
const int TernaryExpr::kind = x::next_kind("ternary_expr");
const int BoolLiteral::kind = x::next_kind("bool_literal");
const int TypeIdent::kind = x::next_kind("type_ident");
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
const int StaticArrayTypename::kind = x::next_kind("static_array_typename");
const int TypeAlias::kind = x::next_kind("type_alias");
const int StructDecl::kind = x::next_kind("struct_decl");
const int VarDecl::kind = x::next_kind("var_decl");
const int VarDeclInit::kind = x::next_kind("var_decl_init");
const int ArrayLiteral::kind = x::next_kind("array_literal");
const int IfStmt::kind = x::next_kind("if_statement");
const int IfElseStmt::kind = x::next_kind("if_else_statement");
const int WhileStmt::kind = x::next_kind("while_statement");
const int ForStmt::kind = x::next_kind("for");
const int AddrOf::kind = x::next_kind("addr_of");
const int Deref::kind = x::next_kind("deref");
const int CastExpr::kind = x::next_kind("cast_expr");
const int LogicalExpr::kind = x::next_kind("logical_expr");
const int TupleExpr::kind = x::next_kind("tuple_expr");
const int FunctionCall::kind = x::next_kind("function_call");
const int StatementList::kind = x::next_kind("statement_list");
const int ParamsList::kind = x::next_kind("params_list");
const int FuncDecl::kind = x::next_kind("func_decl");
const int ReturnStatement::kind = x::next_kind("return_statement");
const int Assignment::kind = x::next_kind("assignment");

int x::next_kind(const char * const name) {
    static int kind = 0;
    x::kind_map.push_back(std::string(name));

    return kind++;
}

template<typename T> static std::vector<ASTNode *> cast_nodes(std::vector<T> &vec) {
    std::vector<ASTNode *> out;

    std::transform(vec.begin(), vec.end(), std::back_inserter(out), [](auto &item){
        return (ASTNode *) item;
    });

    return out;
}

IntLiteral::IntLiteral(const char * int_str) : value(atoi(int_str)) {}

IntLiteral::IntLiteral(const int value) : value(value) {}

void IntLiteral::print() const {
    printf("%d", value);
}

std::vector<ASTNode *> IntLiteral::children() {
    return {};
}

FloatLiteral::FloatLiteral(const char * float_str) : value(atof(float_str)) {}

FloatLiteral::FloatLiteral(const float value) : value(value) {}

void FloatLiteral::print() const {
    printf("%f", value);
}

std::vector<ASTNode *> FloatLiteral::children() {
    return {};
}

BoolLiteral::BoolLiteral(const bool value) : value(value) {}

void BoolLiteral::print() const {
    if (value) {
        printf("true");
    } else {
        printf("false");
    }
}

std::vector<ASTNode *> BoolLiteral::children() {
    return {};
}

TernaryExpr::TernaryExpr(const Expr * cond, const Expr * tru, const Expr * fals) :
    cond(cond),
    tru(tru),
    fals(fals)
    {}

TernaryExpr::~TernaryExpr() {
    delete cond;
    delete tru;
    delete fals;
}

void TernaryExpr::print() const {
    cond->print();
    printf(" ? ");
    tru->print();  printf(" : "); fals->print();
}

std::vector<ASTNode *> TernaryExpr::children() {
    return {(ASTNode *) cond, (ASTNode *) tru, (ASTNode *) fals};
}

TypeIdent::TypeIdent(const char * const _id) : id(std::string(_id)) {}

void TypeIdent::print() const {
    std::cout << id;
}

std::vector<ASTNode *> TypeIdent::children() {
    return {};
}

Ident::Ident(const char * const _id) : id(std::string(_id)) {}

void Ident::print() const {
    std::cout << id;
}

std::vector<ASTNode *> Ident::children() {
    return {};
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

std::vector<ASTNode *> MathExpr::children() {
    return {(ASTNode *) left, (ASTNode *) right};
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

std::vector<ASTNode *> BoolExpr::children() {
    return {(ASTNode *) left, (ASTNode *) right};
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

std::vector<ASTNode *> ParensExpr::children() {
    return {(ASTNode *) expr};
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

std::vector<ASTNode *> ParensTypename::children() {
    return {(ASTNode *) name};
}

PtrTypename::PtrTypename(const Typename * name) : name(name) {}

PtrTypename::~PtrTypename() {
    delete name;
}

void PtrTypename::print() const {
    name->print();
    putchar('*');
}

std::vector<ASTNode *> PtrTypename::children() {
    return {(ASTNode *) name};
}

MutTypename::MutTypename(const Typename * name) : name(name) {}

MutTypename::~MutTypename() {
    delete name;
}

void MutTypename::print() const {
    printf("mut ");
    name->print();
}

std::vector<ASTNode *> MutTypename::children() {
    return {(ASTNode *) name};
}

TypenameList::TypenameList(std::vector<Typename *> types) : types(types) {}

TypenameList::~TypenameList() {
    for (auto &type_name : types) {
        delete type_name;
    }
}

void TypenameList::print() const {
    if (types.size() == 0) {
        return;
    }

    for (size_t i = 0; i < types.size() - 1; i++) {
        types[i]->print();
        printf(", ");
    }

    types[types.size() - 1]->print();
}

void TypenameList::push_type(Typename * type_name) {
    types.push_back(type_name);
}

std::vector<ASTNode *> TypenameList::children() {
    return cast_nodes(types);
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

std::vector<ASTNode *> VarDeclList::children() {
    return cast_nodes(decls);
}

ExprList::ExprList(std::vector<Expr *> exprs) : exprs(exprs) {}

ExprList::~ExprList() {
    exprs.clear();
}

void ExprList::print() const {
    if (exprs.size() == 0) {
        return;
    }

    for (size_t i = 0; i < exprs.size() - 1; i++) {
        exprs[i]->print();
        printf(", ");
    }

    exprs[exprs.size() - 1]->print();
}

void ExprList::push_expr(Expr * expr) {
    exprs.push_back(expr);
}

std::vector<ASTNode *> ExprList::children() {
    return cast_nodes(exprs);
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

std::vector<ASTNode *> StatementList::children() {
    return cast_nodes(statements);
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

std::vector<ASTNode *> TupleTypename::children() {
    return {(ASTNode *) type_list};
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

std::vector<ASTNode *> TupleExpr::children() {
    return {(ASTNode *) expr_list};
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

std::vector<ASTNode *> FuncTypename::children() {
    return {(ASTNode *) params, (ASTNode *) ret_type};
}

StaticArrayTypename::StaticArrayTypename(const Typename * element_type, const IntLiteral * size) :
    element_type(element_type),
    size(size)
    {}

StaticArrayTypename::~StaticArrayTypename() {
    delete element_type;
    delete size;
}

void StaticArrayTypename::print() const {
    element_type->print();
    putchar('[');
    size->print();
    putchar(']');
}

std::vector<ASTNode *> StaticArrayTypename::children() {
    return {(ASTNode *) element_type, (ASTNode *) size};
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

std::vector<ASTNode *> TypeAlias::children() {
    return {(Typename *) name, (ASTNode *) type_expr};
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

std::vector<ASTNode *> StructDecl::children() {
    return {(Expr *) name, (ASTNode *) members};
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

std::vector<ASTNode *> VarDecl::children() {
    return {(ASTNode *) type_name, (Expr *) var_name};
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

std::vector<ASTNode *> VarDeclInit::children() {
    return {(ASTNode *) decl, (ASTNode *) init};
}

ArrayLiteral::ArrayLiteral(const ExprList * items) : items(items) {}

ArrayLiteral::~ArrayLiteral() {
    delete items;
}

void ArrayLiteral::print() const {
    putchar('{');
    items->print();
    putchar('}');
}

std::vector<ASTNode *> ArrayLiteral::children() {
    return {(ASTNode *) items};
}

IfStmt::IfStmt(const Expr * cond, const StatementList * then) :
    cond(cond),
    then(then)
    {}

IfStmt::~IfStmt() {
    delete cond;
    delete then;
}

void IfStmt::print() const {
    printf("if (");
    cond->print();
    printf(") {\n");
    then->print();
    printf("}");
}

std::vector<ASTNode *> IfStmt::children() {
    return {(ASTNode *) cond, (ASTNode *) then};
}

IfElseStmt::IfElseStmt(const IfStmt * if_stmt, const StatementList * els) : if_stmt(if_stmt), els(els) {}

IfElseStmt::~IfElseStmt() {
    delete if_stmt;
    delete els;
}

void IfElseStmt::print() const {
    if_stmt->print();
    printf(" else {\n");
    els->print();
    printf("}");
}

std::vector<ASTNode *> IfElseStmt::children() {
    return {(ASTNode *) if_stmt, (ASTNode *) els};
}

WhileStmt::WhileStmt(const Expr * cond, const StatementList * body) : cond(cond), body(body) {}

WhileStmt::~WhileStmt() {
    delete cond;
    delete body;
}

void WhileStmt::print() const {
    printf("while (");
    cond->print();
    printf(") {\n");
    body->print();
    printf("};\n");
}

std::vector<ASTNode *> WhileStmt::children() {
    return {(ASTNode *) cond, (ASTNode *) body};
}

ForStmt::ForStmt(const Expr * init, const Expr * cond, const Expr * update, const StatementList * body) :
    init(init),
    condition(cond),
    update(update),
    body(body)
    {}

ForStmt::~ForStmt() {
    delete init;
    delete condition;
    delete update;
    delete body;
}

void ForStmt::print() const {
    printf("for (");
    init->print();
    putchar(' ');
    condition->print();
    putchar(' ');
    update->print();
    putchar(' ');
    printf(" {\n");
    body->print();
    printf("}");
}

std::vector<ASTNode *> ForStmt::children() {
    return {(ASTNode *) init, (ASTNode *) condition, (ASTNode *) update, (ASTNode *) body};
}

AddrOf::AddrOf(const Expr * expr) : expr(expr) {}

AddrOf::~AddrOf() {
    delete expr;
}

void AddrOf::print() const {
    putchar('&');
    expr->print();
}

std::vector<ASTNode *> AddrOf::children() {
    return {(ASTNode *) expr};
}

Deref::Deref(const Expr * expr) : expr(expr) {}

Deref::~Deref() {
    delete expr;
}

void Deref::print() const {
    putchar('*');
    expr->print();
}

std::vector<ASTNode *> Deref::children() {
    return {(ASTNode *) expr};
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

std::vector<ASTNode *> CastExpr::children() {
    return {(ASTNode *) dest_type, (ASTNode *) expr};
}

LogicalExpr::LogicalExpr(const std::string op, const Expr *l, const Expr *r) :
    op(op),
    left(l),
    right(r)
    {}

LogicalExpr::~LogicalExpr() {
    delete left;
    delete right;
}

void LogicalExpr::print() const {
    left->print();
    printf(" %s ", op.c_str());
    right->print();
}

std::vector<ASTNode *> LogicalExpr::children() {
    return {(ASTNode *) left, (ASTNode *) right};
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

std::vector<ASTNode *> FunctionCall::children() {
    return {(ASTNode *) func, (ASTNode *) args};
}

ParamsList::ParamsList(std::vector<VarDecl *> params) : params(params) {}

ParamsList::~ParamsList() {
    params.clear();
}

void ParamsList::print() const {
    if (params.size() == 0) {
        return;
    }

    for (size_t i = 0; i < params.size() - 1; i++) {
        params[i]->print();
        printf(", ");
    }

    params[params.size() - 1]->print();
}

void ParamsList::push_param(VarDecl * param) {
    params.push_back(param);
}

void ParamsList::add_to_scope(SymbolTable * symtable) {
    for (auto &param : params) {
        param->add_to_scope(symtable);
    }
}

std::vector<ASTNode *> ParamsList::children() {
    return cast_nodes(params);
}

FuncDecl::FuncDecl(const Ident * name, const ParamsList * params, const Typename * ret_type, const StatementList * body) :
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

std::vector<ASTNode *> FuncDecl::children() {
    return {(Expr *) name, (ASTNode *) params, (ASTNode *) ret_type, (ASTNode *) body};
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

std::vector<ASTNode *> ProgramSource::children() {
    return nodes;
}

ReturnStatement::ReturnStatement(const Expr * val) : val(val) {}

ReturnStatement::~ReturnStatement() {
    delete val;
}

void ReturnStatement::print() const {
    printf("return ");
    val->print();
}

std::vector<ASTNode *> ReturnStatement::children() {
    return {(ASTNode *) val};
}

Assignment::Assignment(const Expr * lhs, const Expr * rhs) : lhs(lhs), rhs(rhs) {}

Assignment::~Assignment() {
    delete lhs;
    delete rhs;
}

void Assignment::print() const {
    lhs->print();
    printf(" = ");
    rhs->print();
}

std::vector<ASTNode *> Assignment::children() {
    return {(ASTNode *) lhs, (ASTNode *) rhs};
}
