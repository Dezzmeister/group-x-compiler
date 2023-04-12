#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "ast.h"

std::vector<std::string> x::kind_map;

const int IntLiteral::kind = x::next_kind("int_literal");
const int FloatLiteral::kind = x::next_kind("float_literal");
const int Ident::kind = x::next_kind("ident");
const int MathOp::kind = x::next_kind("math_op");
const int ParensTypename::kind = x::next_kind("parens_typename");
const int PtrTypename::kind = x::next_kind("ptr_typename");
const int MutTypename::kind = x::next_kind("mut_typename");
const int TypenameList::kind = x::next_kind("typename_list");
const int VarDeclList::kind = x::next_kind("var_decl_list");
const int TupleTypename::kind = x::next_kind("tuple_typename");
const int FuncTypename::kind = x::next_kind("func_typename");
const int TypeAlias::kind = x::next_kind("type_alias");
const int StructDecl::kind = x::next_kind("struct_decl");
const int VarDecl::kind = x::next_kind("var_decl");

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

Ident::Ident(const char * const _id) : id(std::string(_id)) {}

void Ident::print() const {
    std::cout << id;
}

MathOp::MathOp(const char op, std::vector<MathExpr *> operands) : 
    op(op),
    operands(operands)
    {}

MathOp::~MathOp() {
    for (auto &ptr : operands) {
        delete ptr;
    }
}

void MathOp::push_operand(MathExpr * operand) {
    operands.push_back(operand);
}

void MathOp::print() const {
    for (size_t i = 0; i < operands.size() - 1; i++) {
        operands[i]->print();
        putchar(op);
    }

    operands[operands.size() - 1]->print();
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

TupleTypename::TupleTypename(const TypenameList * type_list) : type_list(type_list) {}

TupleTypename::~TupleTypename() {
    delete type_list;
}

void TupleTypename::print() const {
    putchar('[');

    type_list->print();

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
