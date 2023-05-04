#include "ast.h"

#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>

#include "parser.h"
#include "tac.h"

std::vector<std::string> x::kind_map;

const int ProgramSource::kind = x::next_kind("program_source");
const int IntLiteral::kind = x::next_kind("int_literal");
const int FloatLiteral::kind = x::next_kind("float_literal");
const int TernaryExpr::kind = x::next_kind("ternary_expr");
const int BoolLiteral::kind = x::next_kind("bool_literal");
const int CharLiteral::kind = x::next_kind("char_literal");
const int StringLiteral::kind = x::next_kind("str_literal");
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
const int StructTypename::kind = x::next_kind("struct_typename");
const int StructDecl::kind = x::next_kind("struct_decl");
const int VarDecl::kind = x::next_kind("var_decl");
const int VarDeclInit::kind = x::next_kind("var_decl_init");
const int ArrayLiteral::kind = x::next_kind("array_literal");
const int IfStmt::kind = x::next_kind("if");
const int IfElseStmt::kind = x::next_kind("if_else");
const int WhileStmt::kind = x::next_kind("while");
const int ForStmt::kind = x::next_kind("for");
const int AddrOf::kind = x::next_kind("addr_of");
const int Deref::kind = x::next_kind("deref");
const int CastExpr::kind = x::next_kind("cast_expr");
const int LogicalExpr::kind = x::next_kind("logical_expr");
const int TupleExpr::kind = x::next_kind("tuple_expr");
const int FunctionCallExpr::kind = x::next_kind("function_call_expr");
const int FunctionCallStmt::kind = x::next_kind("function_call_stmt");
const int StatementList::kind = x::next_kind("statement_list");
const int ParamsList::kind = x::next_kind("params_list");
const int FuncDecl::kind = x::next_kind("func_decl");
const int ReturnStatement::kind = x::next_kind("return_statement");
const int Assignment::kind = x::next_kind("assignment");
const int BangExpr::kind = x::next_kind("bang_expr");
const int NotExpr::kind = x::next_kind("not_expr");
const int PreExpr::kind = x::next_kind("pre_expr");
const int StructDeref::kind = x::next_kind("struct_deref");
const int MemberInitializer::kind = x::next_kind("member_initializer");
const int InitializerList::kind = x::next_kind("initializer_list");
const int StructLiteral::kind = x::next_kind("struct_literal");
const int ArrayIndexExpr::kind = x::next_kind("array_index_expr");
const int VoidReturnStmt::kind = x::next_kind("void_return_stmt");
const int ContinueStmt::kind = x::next_kind("continue_stmt");
const int BreakStmt::kind = x::next_kind("break_stmt");

int x::next_kind(const char * const name) {
    static int kind = 0;
    x::kind_map.push_back(std::string(name));

    return kind++;
}

template <typename T>
static std::vector<ASTNode *> cast_nodes(std::vector<T> &vec) {
    std::vector<ASTNode *> out;

    std::transform(vec.begin(), vec.end(), std::back_inserter(out),
    [](auto &item) {
        return (ASTNode *)item;
    });

    return out;
}

template <typename T>
static bool cmp_vectors(const std::vector<T *> &v1, const std::vector<T *> &v2) {
    if (v1.size() != v2.size()) {
        return false;
    }

    for (size_t i = 0; i < v1.size(); i++) {
        if (*(v1[i]) != *(v2[i])) {
            return false;
        }
    }

    return true;
}

Location::Location(int first_line, int first_col, int last_line, int last_col) :
    first_line(first_line), first_col(first_col), last_line(last_line), last_col(last_col) {};

Location::Location(YYLTYPE &start, YYLTYPE &end) :
    first_line(start.first_line), first_col(start.first_column), last_line(end.last_line), last_col(end.last_column) {}

Location::Location(const Location &start, const Location &end) :
    first_line(start.first_line), first_col(start.first_col), last_line(end.last_line), last_col(end.last_col) {}

void Location::set_end(YYLTYPE &end) {
    last_line = end.last_line;
    last_col = end.last_column;
}

ASTNode * ASTNode::find(FindFunc cond) {
    if (cond(this)) {
        return this;
    }

    for (auto &child : children()) {
        ASTNode * result = child->find(cond);

        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

IntLiteral::IntLiteral(const Location loc, const char * int_str) :
    NumLiteral(loc), value(atoi(int_str)) {}

IntLiteral::IntLiteral(const Location loc, const int value) :
    NumLiteral(loc), value(value) {}

void IntLiteral::print() const {
    printf("%d", value);
}

std::vector<ASTNode *> IntLiteral::children() {
    return {};
}

bool IntLiteral::operator==(const ASTNode &node) const {
    if (node.get_kind() != IntLiteral::kind) {
        return false;
    }

    return value == ((IntLiteral &) node).value;
}

FloatLiteral::FloatLiteral(const Location loc, const char * float_str) :
    NumLiteral(loc), value(atof(float_str)) {}

FloatLiteral::FloatLiteral(const Location loc, const float value) :
    NumLiteral(loc), value(value) {}

void FloatLiteral::print() const {
    printf("%f", value);
}

std::vector<ASTNode *> FloatLiteral::children() {
    return {};
}

bool FloatLiteral::operator==(const ASTNode &node) const {
    if (node.get_kind() != FloatLiteral::kind) {
        return false;
    }

    return value == ((FloatLiteral &) node).value;
}

BoolLiteral::BoolLiteral(const Location loc, const bool value) : Expr(loc), value(value) {}

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

bool BoolLiteral::operator==(const ASTNode &node) const {
    if (node.get_kind() != BoolLiteral::kind) {
        return false;
    }

    return value == ((BoolLiteral &) node).value;
}

CharLiteral::CharLiteral(const Location loc, const char value) : Expr(loc), value(value) {}

void CharLiteral::print() const {
    switch (value) {
        case '\n':
            printf("\\n");
            break;

        case '\t':
            printf("\\t");
            break;

        case '\0':
            printf("\\0");
            break;

        case '\e':
            printf("\\e");
            break;

        case '\r':
            printf("\\r");
            break;

        default:
            printf("'%c'", value);
    }
}

std::vector<ASTNode *> CharLiteral::children() {
    return {};
}

bool CharLiteral::operator==(const ASTNode &node) const {
    if (node.get_kind() != CharLiteral::kind) {
        return false;
    }

    return value == ((CharLiteral &) node).value;
}

StringLiteral::StringLiteral(const Location loc, const char * const value)
    : Expr(loc), value(std::string(value)) {}

void StringLiteral::print() const {
    putchar('"');
    std::cout << value;
    putchar('"');
}

std::vector<ASTNode *> StringLiteral::children() {
    return {};
}

bool StringLiteral::operator==(const ASTNode &node) const {
    if (node.get_kind() != StringLiteral::kind) {
        return false;
    }

    return value == ((StringLiteral &) node).value;
}

TernaryExpr::TernaryExpr(const Location loc, const Expr * cond, const Expr * tru, const Expr * fals)
    : Expr(loc), cond(cond), tru(tru), fals(fals) {}

TernaryExpr::~TernaryExpr() {
    delete cond;
    delete tru;
    delete fals;
}

void TernaryExpr::print() const {
    cond->print();
    printf(" ? ");
    tru->print();
    printf(" : ");
    fals->print();
}

std::vector<ASTNode *> TernaryExpr::children() {
    return {(ASTNode *)cond, (ASTNode *)tru, (ASTNode *)fals};
}

bool TernaryExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != TernaryExpr::kind) {
        return false;
    }

    const TernaryExpr &n = (TernaryExpr &) node;

    return (*cond == *(n.cond) && *tru == *(n.tru) && *fals == *(n.fals));
}

TypeIdent::TypeIdent(const Location loc, const char * const _id) :
    Typename(loc), id(std::string(_id)) {}

Typename * TypeIdent::clone() const {
    return new TypeIdent(loc, id.c_str());
}

void TypeIdent::print() const {
    std::cout << id;
}

std::vector<ASTNode *> TypeIdent::children() {
    return {};
}

bool TypeIdent::operator==(const ASTNode &node) const {
    if (node.get_kind() != TypeIdent::kind) {
        return false;
    }

    const TypeIdent &n = (TypeIdent &) node;

    return (id == n.id);
}

Ident::Ident(const Location loc, const char * const _id) :
    CallingExpr(loc), id(std::string(_id)) {}

void Ident::print() const {
    std::cout << id;
}

std::vector<ASTNode *> Ident::children() {
    return {};
}

bool Ident::operator==(const ASTNode &node) const {
    if (node.get_kind() != Ident::kind) {
        return false;
    }

    const Ident &n = (Ident &) node;

    return (id == n.id);
}

MathExpr::MathExpr(const Location loc, const char op, const Expr * left, const Expr * right)
    : Expr(loc), op(op), left(left), right(right) {}

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
    return {(ASTNode *)left, (ASTNode *)right};
}

bool MathExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != MathExpr::kind) {
        return false;
    }

    const MathExpr &n = (MathExpr &) node;

    return (op == n.op && *left == *(n.left) && *right == *(n.right));
}

BoolExpr::BoolExpr(const Location loc, const char * const op, const Expr * left, const Expr * right)
    : Expr(loc), op(std::string(op)), left(left), right(right) {}

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
    return {(ASTNode *)left, (ASTNode *)right};
}

bool BoolExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != BoolExpr::kind) {
        return false;
    }

    const BoolExpr &n = (BoolExpr &) node;

    return (op == n.op && *left == *(n.left) && *right == *(n.right));
}

ParensExpr::ParensExpr(const Location loc, const Expr * expr) :
    CallingExpr(loc), expr(expr) {}

ParensExpr::~ParensExpr() {
    delete expr;
}

void ParensExpr::print() const {
    putchar('(');
    expr->print();
    putchar(')');
}

std::vector<ASTNode *> ParensExpr::children() {
    return {(ASTNode *)expr};
}

bool ParensExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != ParensExpr::kind) {
        return false;
    }

    const ParensExpr &n = (ParensExpr &) node;

    return (*expr == *(n.expr));
}

ParensTypename::ParensTypename(const Location loc, const Typename * name) :
    Typename(loc), name(name) {}

Typename * ParensTypename::clone() const {
    return new ParensTypename(loc, name->clone());
}

ParensTypename::~ParensTypename() {
    delete name;
}

void ParensTypename::print() const {
    putchar('(');
    name->print();
    putchar(')');
}

std::vector<ASTNode *> ParensTypename::children() {
    return {(ASTNode *)name};
}

bool ParensTypename::operator==(const ASTNode &node) const {
    if (node.get_kind() != ParensTypename::kind) {
        return false;
    }

    const ParensTypename &n = (ParensTypename &) node;

    return (*name == *(n.name));
}

PtrTypename::PtrTypename(const Location loc, const Typename * name) :
    Typename(loc), name(name) {}

Typename * PtrTypename::clone() const {
    return new PtrTypename(loc, name->clone());
}

PtrTypename::~PtrTypename() {
    delete name;
}

void PtrTypename::print() const {
    name->print();
    putchar('*');
}

std::vector<ASTNode *> PtrTypename::children() {
    return {(ASTNode *)name};
}

bool PtrTypename::operator==(const ASTNode &node) const {
    if (node.get_kind() != PtrTypename::kind) {
        return false;
    }

    const PtrTypename &n = (PtrTypename &) node;

    return (*name == *(n.name));
}

MutTypename::MutTypename(const Location loc, const Typename * name) :
    Typename(loc), name(name) {}

Typename * MutTypename::clone() const {
    return new MutTypename(loc, name->clone());
}

MutTypename::~MutTypename() {
    delete name;
}

void MutTypename::print() const {
    printf("mut ");
    name->print();
}

std::vector<ASTNode *> MutTypename::children() {
    return {(ASTNode *)name};
}

bool MutTypename::operator==(const ASTNode &node) const {
    if (node.get_kind() != MutTypename::kind) {
        return false;
    }

    const MutTypename &n = (MutTypename &) node;

    return (*name == *(n.name));
}

TypenameList::TypenameList(const Location loc, std::vector<Typename *> types) :
    ASTNode(loc), types(types) {}

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

bool TypenameList::operator==(const ASTNode &node) const {
    if (node.get_kind() != TypenameList::kind) {
        return false;
    }

    const TypenameList &n = (TypenameList &) node;

    return cmp_vectors(types, n.types);
}

VarDeclList::VarDeclList(const Location loc, std::vector<VarDecl *> decls) :
    ASTNode(loc), decls(decls) {}

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

bool VarDeclList::operator==(const ASTNode &node) const {
    if (node.get_kind() != VarDeclList::kind) {
        return false;
    }

    const VarDeclList &n = (VarDeclList &) node;

    return cmp_vectors(decls, n.decls);
}

ExprList::ExprList(const Location loc, std::vector<Expr *> exprs) :
    ASTNode(loc), exprs(exprs) {}

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

bool ExprList::operator==(const ASTNode &node) const {
    if (node.get_kind() != ExprList::kind) {
        return false;
    }

    const ExprList &n = (ExprList &) node;

    return cmp_vectors(exprs, n.exprs);
}

StatementList::StatementList(const Location loc, std::vector<Statement *> statements)
    : ASTNode(loc), statements(statements) {}

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

bool StatementList::operator==(const ASTNode &node) const {
    if (node.get_kind() != StatementList::kind) {
        return false;
    }

    const StatementList &n = (StatementList &) node;

    return cmp_vectors(statements, n.statements);
}

TupleTypename::TupleTypename(const Location loc, const TypenameList * type_list)
    : Typename(loc), type_list(type_list) {}

Typename * TupleTypename::clone() const {
    std::vector<Typename *> types = {};

    for (auto &type_name : type_list->types) {
        types.push_back(type_name->clone());
    }

    return new TupleTypename(loc, new TypenameList(x::NULL_LOC, types));
}

TupleTypename::~TupleTypename() {
    delete type_list;
}

void TupleTypename::print() const {
    putchar('[');
    type_list->print();
    putchar(']');
}

std::vector<ASTNode *> TupleTypename::children() {
    return {(ASTNode *)type_list};
}

bool TupleTypename::operator==(const ASTNode &node) const {
    if (node.get_kind() != TupleTypename::kind) {
        return false;
    }

    const TupleTypename &n = (TupleTypename &) node;

    return (*type_list == *(n.type_list));
}

TupleExpr::TupleExpr(const Location loc, const ExprList * expr_list) :
    Expr(loc), expr_list(expr_list) {}

TupleExpr::~TupleExpr() {
    delete expr_list;
}

void TupleExpr::print() const {
    putchar('[');
    expr_list->print();
    putchar(']');
}

std::vector<ASTNode *> TupleExpr::children() {
    return {(ASTNode *)expr_list};
}

bool TupleExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != TupleExpr::kind) {
        return false;
    }

    const TupleExpr &n = (TupleExpr &) node;

    return (*expr_list == *(n.expr_list));
}

FuncTypename::FuncTypename(const Location loc, const TypenameList * params, const Typename * ret_type)
    : Typename(loc), params(params), ret_type(ret_type) {}

Typename * FuncTypename::clone() const {
    const Typename * ret_clone = ret_type->clone();
    std::vector<Typename *> params_clone = {};

    for (auto &type_name : params->types) {
        params_clone.push_back(type_name->clone());
    }

    return new FuncTypename(loc, new TypenameList(x::NULL_LOC, params_clone), ret_clone);
}

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
    return {(ASTNode *)params, (ASTNode *)ret_type};
}

bool FuncTypename::operator==(const ASTNode &node) const {
    if (node.get_kind() != FuncTypename::kind) {
        return false;
    }

    const FuncTypename &n = (FuncTypename &) node;

    return (*params == *(n.params) && *ret_type == *(n.ret_type));
}

StaticArrayTypename::StaticArrayTypename(const Location loc, const Typename * element_type,
        const IntLiteral * size)
    : Typename(loc), element_type(element_type), size(size) {}

Typename * StaticArrayTypename::clone() const {
    return new StaticArrayTypename(loc, element_type->clone(), new IntLiteral(size->loc, size->value));
}

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
    return {(ASTNode *)element_type, (ASTNode *)size};
}

bool StaticArrayTypename::operator==(const ASTNode &node) const {
    if (node.get_kind() != StaticArrayTypename::kind) {
        return false;
    }

    const StaticArrayTypename &n = (StaticArrayTypename &) node;

    return (*element_type == *(n.element_type) && *size == *(n.size));
}

TypeAlias::TypeAlias(const Location loc, const Ident * name, const Typename * type_expr)
    : TypeDecl(loc), name(name), type_expr(type_expr) {}

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
    return {(Typename *)name, (ASTNode *)type_expr};
}

bool TypeAlias::operator==(const ASTNode &node) const {
    if (node.get_kind() != TypeAlias::kind) {
        return false;
    }

    const TypeAlias &n = (TypeAlias &) node;

    return (*name == *(n.name) && *type_expr == *(n.type_expr));
}

StructTypename::StructTypename(const Location loc, const VarDeclList * members, SymbolTable * scope)
    : Typename(loc), members(members), scope(scope) {}

Typename * StructTypename::clone() const {
    std::vector<VarDecl *> members_clone = {};

    for (auto &member : members->decls) {
        Typename * type_name_clone = member->type_name->clone();
        Ident * var_name_clone = new Ident(member->var_name->loc, member->var_name->id.c_str());

        members_clone.push_back(new VarDecl(member->loc, type_name_clone, var_name_clone));
    }

    return new StructTypename(loc, new VarDeclList(members->loc, members_clone), scope);
}

StructTypename::~StructTypename() {
    delete members;
}

void StructTypename::print() const {
    printf("{\n");
    members->print();
    printf("};\n");
}

std::vector<ASTNode *> StructTypename::children() {
    return {(ASTNode *)members};
}

bool StructTypename::operator==(const ASTNode &node) const {
    if (node.get_kind() != StructTypename::kind) {
        return false;
    }

    const StructTypename &n = (StructTypename &) node;

    return (*members == *(n.members));
}

StructDecl::StructDecl(const Location loc, const Ident * name, const StructTypename * defn)
    : TypeDecl(loc), name(name), defn(defn) {}

StructDecl::~StructDecl() {
    delete name;
    delete defn;
}

void StructDecl::print() const {
    printf("struct ");
    name->print();
    putchar(' ');
    defn->print();
}

std::vector<ASTNode *> StructDecl::children() {
    return {(Expr *)name, (ASTNode *)defn};
}

bool StructDecl::operator==(const ASTNode &node) const {
    if (node.get_kind() != StructDecl::kind) {
        return false;
    }

    const StructDecl &n = (StructDecl &) node;

    return (*name == *(n.name) && *defn == *(n.defn));
}

VarDecl::VarDecl(const Location loc, const Typename * type_name, const Ident * var_name)
    : Statement(loc), type_name(type_name), var_name(var_name) {}

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
    symtable->put(var_name->id, new Symbol(Var, (Decl) {
        .var=this
    }));
}

std::vector<ASTNode *> VarDecl::children() {
    return {(ASTNode *)type_name, (Expr *)var_name};
}

bool VarDecl::operator==(const ASTNode &node) const {
    if (node.get_kind() != VarDecl::kind) {
        return false;
    }

    const VarDecl &n = (VarDecl &) node;

    return (*type_name == *(n.type_name) && *var_name == *(n.var_name));
}

VarDeclInit::VarDeclInit(const Location loc, const VarDecl * decl, const Expr * init)
    : Statement(loc), decl(decl), init(init) {}

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
    return {(ASTNode *)decl, (ASTNode *)init};
}

bool VarDeclInit::operator==(const ASTNode &node) const {
    if (node.get_kind() != VarDeclInit::kind) {
        return false;
    }

    const VarDeclInit &n = (VarDeclInit &) node;

    return (*decl == *(n.decl) && *init == *(n.init));
}

ArrayLiteral::ArrayLiteral(const Location loc, const ExprList * items) :
    Expr(loc), items(items) {}

ArrayLiteral::~ArrayLiteral() {
    delete items;
}

void ArrayLiteral::print() const {
    putchar('{');
    items->print();
    putchar('}');
}

std::vector<ASTNode *> ArrayLiteral::children() {
    return {(ASTNode *)items};
}

bool ArrayLiteral::operator==(const ASTNode &node) const {
    if (node.get_kind() != ArrayLiteral::kind) {
        return false;
    }

    const ArrayLiteral &n = (ArrayLiteral &) node;

    return (*items == *(n.items));
}

IfStmt::IfStmt(const Location loc, const Expr * cond, const StatementList * then, SymbolTable * scope)
    : Statement(loc), cond(cond), then(then), scope(scope) {}

IfStmt::~IfStmt() {
    delete cond;
    delete then;
    delete scope;
}

void IfStmt::print() const {
    printf("if (");
    cond->print();
    printf(") {\n");
    then->print();
    printf("}");
}

std::vector<ASTNode *> IfStmt::children() {
    return {(ASTNode *)cond, (ASTNode *)then};
}

bool IfStmt::operator==(const ASTNode &node) const {
    if (node.get_kind() != IfStmt::kind) {
        return false;
    }

    const IfStmt &n = (IfStmt &) node;

    return (*cond == *(n.cond) && *then == *(n.then));
}

IfElseStmt::IfElseStmt(const Location loc, const IfStmt * if_stmt, const StatementList * els, SymbolTable * scope)
    : Statement(loc), if_stmt(if_stmt), els(els), scope(scope) {}

IfElseStmt::~IfElseStmt() {
    delete if_stmt;
    delete els;
    delete scope;
}

void IfElseStmt::print() const {
    if_stmt->print();
    printf(" else {\n");
    els->print();
    printf("}");
}

std::vector<ASTNode *> IfElseStmt::children() {
    return {(ASTNode *)if_stmt, (ASTNode *)els};
}

bool IfElseStmt::operator==(const ASTNode &node) const {
    if (node.get_kind() != IfElseStmt::kind) {
        return false;
    }

    const IfElseStmt &n = (IfElseStmt &) node;

    return (*if_stmt == *(n.if_stmt) && *els == *(n.els));
}

WhileStmt::WhileStmt(const Location loc, const Expr * cond, const StatementList * body, SymbolTable * scope)
    : Statement(loc), cond(cond), body(body), scope(scope) {}

WhileStmt::~WhileStmt() {
    delete cond;
    delete body;
    delete scope;
}

void WhileStmt::print() const {
    printf("while (");
    cond->print();
    printf(") {\n");
    body->print();
    printf("};\n");
}

std::vector<ASTNode *> WhileStmt::children() {
    return {(ASTNode *)cond, (ASTNode *)body};
}

bool WhileStmt::operator==(const ASTNode &node) const {
    if (node.get_kind() != WhileStmt::kind) {
        return false;
    }

    const WhileStmt &n = (WhileStmt &) node;

    return (*cond == *(n.cond) && *body == *(n.body));
}

ForStmt::ForStmt(const Location loc, const Statement * init, const Expr * cond,
                 const Statement * update, const StatementList * body, SymbolTable * scope)
    : Statement(loc), init(init), condition(cond), update(update), body(body), scope(scope) {}

ForStmt::~ForStmt() {
    delete init;
    delete condition;
    delete update;
    delete body;
    delete scope;
}

void ForStmt::print() const {
    printf("for (");
    init->print();
    printf("; ");
    condition->print();
    printf("; ");
    update->print();
    printf(") {\n");
    body->print();
    printf("}");
}

std::vector<ASTNode *> ForStmt::children() {
    return {(ASTNode *)init, (ASTNode *)condition, (ASTNode *)update,
            (ASTNode *)body};
}

bool ForStmt::operator==(const ASTNode &node) const {
    if (node.get_kind() != ForStmt::kind) {
        return false;
    }

    const ForStmt &n = (ForStmt &) node;

    return (*init == *(n.init) && *condition == *(n.condition) && *update == *(n.update) && *body == *(n.body));
}

AddrOf::AddrOf(const Location loc, const Expr * expr) :
    Expr(loc), expr(expr) {}

AddrOf::~AddrOf() {
    delete expr;
}

void AddrOf::print() const {
    putchar('&');
    expr->print();
}

std::vector<ASTNode *> AddrOf::children() {
    return {(ASTNode *)expr};
}

bool AddrOf::operator==(const ASTNode &node) const {
    if (node.get_kind() != AddrOf::kind) {
        return false;
    }

    const AddrOf &n = (AddrOf &) node;

    return (*expr == *(n.expr));
}

Deref::Deref(const Location loc, const Expr * expr) :
    Expr(loc), expr(expr) {}

Deref::~Deref() {
    delete expr;
}

void Deref::print() const {
    putchar('*');
    expr->print();
}

std::vector<ASTNode *> Deref::children() {
    return {(ASTNode *)expr};
}

bool Deref::operator==(const ASTNode &node) const {
    if (node.get_kind() != Deref::kind) {
        return false;
    }

    const Deref &n = (Deref &) node;

    return (*expr == *(n.expr));
}

CastExpr::CastExpr(const Location loc, const Typename * dest_type, const Expr * expr)
    : Expr(loc), dest_type(dest_type), expr(expr) {}

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
    return {(ASTNode *)dest_type, (ASTNode *)expr};
}

bool CastExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != CastExpr::kind) {
        return false;
    }

    const CastExpr &n = (CastExpr &) node;

    return (*dest_type == *(n.dest_type) && *expr == *(n.expr));
}

LogicalExpr::LogicalExpr(const Location loc, const char * const op, const Expr * l, const Expr * r)
    : Expr(loc), op(std::string(op)), left(l), right(r) {}

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
    return {(ASTNode *)left, (ASTNode *)right};
}

bool LogicalExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != LogicalExpr::kind) {
        return false;
    }

    const LogicalExpr &n = (LogicalExpr &) node;

    return (op == n.op && *left == *(n.left) && *right == *(n.right));
}

FunctionCallExpr::FunctionCallExpr(const Location loc, const CallingExpr * func, const ExprList * args)
    : CallingExpr(loc), func(func), args(args) {}

FunctionCallExpr::~FunctionCallExpr() {
    delete func;
    delete args;
}

void FunctionCallExpr::print() const {
    func->print();
    putchar('(');
    args->print();
    putchar(')');
}

std::vector<ASTNode *> FunctionCallExpr::children() {
    return {(ASTNode *)func, (ASTNode *)args};
}

bool FunctionCallExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != FunctionCallExpr::kind) {
        return false;
    }

    const FunctionCallExpr &n = (FunctionCallExpr &) node;

    return (*func == *(n.func) && *args == *(n.args));
}

FunctionCallStmt::FunctionCallStmt(const Location loc, const CallingExpr * func, const ExprList * args)
    : Statement(loc), func(func), args(args) {}

FunctionCallStmt::~FunctionCallStmt() {
    delete func;
    delete args;
}

void FunctionCallStmt::print() const {
    func->print();
    putchar('(');
    args->print();
    putchar(')');
}

std::vector<ASTNode *> FunctionCallStmt::children() {
    return {(ASTNode *)func, (ASTNode *)args};
}

bool FunctionCallStmt::operator==(const ASTNode &node) const {
    if (node.get_kind() != FunctionCallStmt::kind) {
        return false;
    }

    const FunctionCallStmt &n = (FunctionCallStmt &) node;

    return (*func == *(n.func) && *args == *(n.args));
}

ParamsList::ParamsList(const Location loc, std::vector<VarDecl *> params) :
    ASTNode(loc), params(params) {}

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

bool ParamsList::operator==(const ASTNode &node) const {
    if (node.get_kind() != ParamsList::kind) {
        return false;
    }

    const ParamsList &n = (ParamsList &) node;

    return cmp_vectors(params, n.params);
}

FuncDecl::FuncDecl(const Location loc, const Ident * name, const ParamsList * params,
                   const Typename * ret_type, const StatementList * body, SymbolTable * scope)
    : ASTNode(loc), name(name), params(params), ret_type(ret_type), body(body), scope(scope), forward_decl(nullptr) {}

FuncDecl::~FuncDecl() {
    delete name;
    delete params;
    delete ret_type;
    delete body;
    delete scope;
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
    return {(Expr *)name, (ASTNode *)params, (ASTNode *)ret_type,
            (ASTNode *)body};
}

bool FuncDecl::operator==(const ASTNode &node) const {
    if (node.get_kind() != FuncDecl::kind) {
        return false;
    }

    const FuncDecl &n = (FuncDecl &) node;

    return (*name == *(n.name) && *params == *(n.params) && *ret_type == *(n.ret_type) && *body == *(n.body));
}

ProgramSource::ProgramSource(const Location loc, std::string name, std::vector<ASTNode *> nodes) :
    ASTNode(loc), name(name), nodes(nodes) {}

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

bool ProgramSource::operator==(const ASTNode &node) const {
    if (node.get_kind() != ProgramSource::kind) {
        return false;
    }

    const ProgramSource &n = (ProgramSource &) node;

    return cmp_vectors(nodes, n.nodes);
}

ReturnStatement::ReturnStatement(const Location loc, const Expr * val) :
    Statement(loc), val(val) {}

ReturnStatement::~ReturnStatement() {
    delete val;
}

void ReturnStatement::print() const {
    printf("return ");
    val->print();
}

std::vector<ASTNode *> ReturnStatement::children() {
    return {(ASTNode *)val};
}

bool ReturnStatement::operator==(const ASTNode &node) const {
    if (node.get_kind() != ReturnStatement::kind) {
        return false;
    }

    const ReturnStatement &n = (ReturnStatement &) node;

    return (*val == *(n.val));
}

Assignment::Assignment(const Location loc, const Expr * lhs, const Expr * rhs) :
    Statement(loc), lhs(lhs), rhs(rhs) {}

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
    return {(ASTNode *)lhs, (ASTNode *)rhs};
}

bool Assignment::operator==(const ASTNode &node) const {
    if (node.get_kind() != Assignment::kind) {
        return false;
    }

    const Assignment &n = (Assignment &) node;

    return (*lhs == *(n.lhs) && *rhs == *(n.rhs));
}

BangExpr::BangExpr(const Location loc, const Expr * expr) :
    Expr(loc), expr(expr) {}

BangExpr::~BangExpr() {
    delete expr;
}

void BangExpr::print() const {
    putchar('!');
    expr->print();
}

std::vector<ASTNode *> BangExpr::children() {
    return {(ASTNode *)expr};
}

bool BangExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != BangExpr::kind) {
        return false;
    }

    const BangExpr &n = (BangExpr &) node;

    return (*expr == *(n.expr));
}

NotExpr::NotExpr(const Location loc, const Expr * expr) :
    Expr(loc), expr(expr) {}

NotExpr::~NotExpr() {
    delete expr;
}

void NotExpr::print() const {
    printf("not ");
    expr->print();
}

std::vector<ASTNode *> NotExpr::children() {
    return {(ASTNode *)expr};
}

bool NotExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != NotExpr::kind) {
        return false;
    }

    const NotExpr &n = (NotExpr &) node;

    return (*expr == *(n.expr));
}

PreExpr::PreExpr(const Location loc, const char * const op, const Expr * expr)
    : Expr(loc), op(std::string(op)), expr(expr) {}

PreExpr::~PreExpr() {
    delete expr;
}

void PreExpr::print() const {
    std::cout << op;
    expr->print();
}

std::vector<ASTNode *> PreExpr::children() {
    return {(ASTNode *)expr};
}

bool PreExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != PreExpr::kind) {
        return false;
    }

    const PreExpr &n = (PreExpr &) node;

    return (op == n.op && *expr == *(n.expr));
}

StructDeref::StructDeref(const Location loc, const CallingExpr * strukt, const Ident * member)
    : CallingExpr(loc), strukt(strukt), member(member) {}

StructDeref::~StructDeref() {
    delete strukt;
    delete member;
}

void StructDeref::print() const {
    strukt->print();
    putchar('.');
    member->print();
}

std::vector<ASTNode *> StructDeref::children() {
    return {(ASTNode *) strukt, (ASTNode *) member};
}

bool StructDeref::operator==(const ASTNode &node) const {
    if (node.get_kind() != StructDeref::kind) {
        return false;
    }

    const StructDeref &n = (StructDeref &) node;

    return (*strukt == *(n.strukt) && *member == *(n.member));
}

MemberInitializer::MemberInitializer(const Location loc, const Ident * member, const Expr * expr)
    : ASTNode(loc), member(member), expr(expr) {}

MemberInitializer::~MemberInitializer() {
    delete member;
    delete expr;
}

void MemberInitializer::print() const {
    member->print();
    printf(": ");
    expr->print();
}

std::vector<ASTNode *> MemberInitializer::children() {
    return {(ASTNode *) member, (ASTNode *) expr};
}

bool MemberInitializer::operator==(const ASTNode &node) const {
    if (node.get_kind() != MemberInitializer::kind) {
        return false;
    }

    const MemberInitializer &n = (MemberInitializer &) node;

    return (*member == *(n.member) && *expr == *(n.expr));
}

InitializerList::InitializerList(const Location loc, std::vector<MemberInitializer *> members)
    : ASTNode(loc), members(members) {}

InitializerList::~InitializerList() {
    members.clear();
}

void InitializerList::print() const {
    for (size_t i = 0; i < members.size() - 1; i++) {
        members[i]->print();
        putchar('\n');
    }

    members[members.size() - 1]->print();
    putchar('\n');
}

std::vector<ASTNode *> InitializerList::children() {
    return cast_nodes(members);
}

bool InitializerList::operator==(const ASTNode &node) const {
    if (node.get_kind() != InitializerList::kind) {
        return false;
    }

    const InitializerList &n = (InitializerList &) node;

    if (members.size() != n.members.size()) {
        return false;
    }

    for (size_t i = 0; i < members.size(); i++) {
        if (*(members[i]) != *(n.members[i])) {
            return false;
        }
    }

    return true;
}

StructLiteral::StructLiteral(const Location loc, const InitializerList * members)
    : CallingExpr(loc), members(members) {}

StructLiteral::~StructLiteral() {
    delete members;
}

void StructLiteral::print() const {
    printf("{\n");
    members->print();
    putchar('}');
}

std::vector<ASTNode *> StructLiteral::children() {
    return {(ASTNode *) members};
}

bool StructLiteral::operator==(const ASTNode &node) const {
    if (node.get_kind() != StructLiteral::kind) {
        return false;
    }

    const StructLiteral &n = (StructLiteral &) node;

    return (*members == *(n.members));
}

ArrayIndexExpr::ArrayIndexExpr(const Location loc, const CallingExpr * arr, const Expr * index)
    : CallingExpr(loc), arr(arr), index(index) {}

ArrayIndexExpr::~ArrayIndexExpr() {
    delete arr;
    delete index;
}

void ArrayIndexExpr::print() const {
    arr->print();
    putchar('[');
    index->print();
    putchar(']');
}

std::vector<ASTNode *> ArrayIndexExpr::children() {
    return {(ASTNode *) arr, (ASTNode *) index};
}

bool ArrayIndexExpr::operator==(const ASTNode &node) const {
    if (node.get_kind() != ArrayIndexExpr::kind) {
        return false;
    }

    const ArrayIndexExpr &n = (ArrayIndexExpr &) node;

    return (*arr == *(n.arr) && *index == *(n.index));
}

VoidReturnStmt::VoidReturnStmt(const Location loc) : Statement(loc) {}

void VoidReturnStmt::print() const {
    printf("return");
}

std::vector<ASTNode *> VoidReturnStmt::children() {
    return {};
}

bool VoidReturnStmt::operator==(const ASTNode &node) const {
    return (node.get_kind() == VoidReturnStmt::kind);
}

ContinueStmt::ContinueStmt(const Location loc) : Statement(loc) {}

void ContinueStmt::print() const {
    printf("continue");
}

std::vector<ASTNode *> ContinueStmt::children() {
    return {};
}

bool ContinueStmt::operator==(const ASTNode &node) const {
    return (node.get_kind() == ContinueStmt::kind);
}

BreakStmt::BreakStmt(const Location loc) : Statement(loc) {}

void BreakStmt::print() const {
    printf("break");
}

std::vector<ASTNode *> BreakStmt::children() {
    return {};
}

bool BreakStmt::operator==(const ASTNode &node) const {
    return (node.get_kind() == BreakStmt::kind);
}
