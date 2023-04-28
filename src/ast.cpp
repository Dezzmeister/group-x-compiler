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

int x::next_kind(const char *const name) {
  static int kind = 0;
  x::kind_map.push_back(std::string(name));

  return kind++;
}

template <typename T>
static std::vector<ASTNode *> cast_nodes(std::vector<T> &vec) {
  std::vector<ASTNode *> out;

  std::transform(vec.begin(), vec.end(), std::back_inserter(out),
                 [](auto &item) { return (ASTNode *)item; });

  return out;
}

template <typename T>
static bool cmp_vectors(const std::vector<T*> &v1, const std::vector<T*> &v2) {
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

IntLiteral::IntLiteral(const char *int_str) : value(atoi(int_str)) {}

IntLiteral::IntLiteral(const int value) : value(value) {}

void IntLiteral::print() const { printf("%d", value); }

std::vector<ASTNode *> IntLiteral::children() { return {}; }

bool IntLiteral::operator==(const ASTNode &node) const {
  if (node.get_kind() != IntLiteral::kind) {
    return false;
  }

  return value == ((IntLiteral&) node).value;
}

FloatLiteral::FloatLiteral(const char *float_str) : value(atof(float_str)) {}

FloatLiteral::FloatLiteral(const float value) : value(value) {}

void FloatLiteral::print() const { printf("%f", value); }

std::vector<ASTNode *> FloatLiteral::children() { return {}; }

bool FloatLiteral::operator==(const ASTNode &node) const {
  if (node.get_kind() != FloatLiteral::kind) {
    return false;
  }

  return value == ((FloatLiteral&) node).value;
}

BoolLiteral::BoolLiteral(const bool value) : value(value) {}

void BoolLiteral::print() const {
  if (value) {
    printf("true");
  } else {
    printf("false");
  }
}

std::vector<ASTNode *> BoolLiteral::children() { return {}; }

bool BoolLiteral::operator==(const ASTNode &node) const {
  if (node.get_kind() != BoolLiteral::kind) {
    return false;
  }

  return value == ((BoolLiteral&) node).value;
}

CharLiteral::CharLiteral(const char value) : value(value) {}

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

std::vector<ASTNode *> CharLiteral::children() { return {}; }

bool CharLiteral::operator==(const ASTNode &node) const {
  if (node.get_kind() != CharLiteral::kind) {
    return false;
  }

  return value == ((CharLiteral&) node).value;
}

StringLiteral::StringLiteral(const char *const value)
    : value(std::string(value)) {}

void StringLiteral::print() const {
  putchar('"');
  std::cout << value;
  putchar('"');
}

std::vector<ASTNode *> StringLiteral::children() { return {}; }

bool StringLiteral::operator==(const ASTNode &node) const {
  if (node.get_kind() != StringLiteral::kind) {
    return false;
  }

  return value == ((StringLiteral&) node).value;
}

TernaryExpr::TernaryExpr(const Expr *cond, const Expr *tru, const Expr *fals)
    : cond(cond), tru(tru), fals(fals) {}

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

  const TernaryExpr &n = (TernaryExpr&) node;

  return (*cond == *(n.cond) && *tru == *(n.tru) && *fals == *(n.fals));
}

TypeIdent::TypeIdent(const char *const _id) : id(std::string(_id)) {}

void TypeIdent::print() const { std::cout << id; }

std::vector<ASTNode *> TypeIdent::children() { return {}; }

bool TypeIdent::operator==(const ASTNode &node) const {
  if (node.get_kind() != TypeIdent::kind) {
    return false;
  }

  const TypeIdent& n = (TypeIdent&) node;

  return (id == n.id);
}

Ident::Ident(const char *const _id) : id(std::string(_id)) {}

void Ident::print() const { std::cout << id; }

std::vector<ASTNode *> Ident::children() { return {}; }

bool Ident::operator==(const ASTNode &node) const {
  if (node.get_kind() != Ident::kind) {
    return false;
  }

  const Ident& n = (Ident&) node;

  return (id == n.id);
}

MathExpr::MathExpr(const char op, const Expr *left, const Expr *right)
    : op(op), left(left), right(right) {}

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

  const MathExpr& n = (MathExpr&) node;

  return (op == n.op && *left == *(n.left) && *right == *(n.right));
}

BoolExpr::BoolExpr(const char *const op, const Expr *left, const Expr *right)
    : op(std::string(op)), left(left), right(right) {}

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

  const BoolExpr& n = (BoolExpr&) node;

  return (op == n.op && *left == *(n.left) && *right == *(n.right));
}

ParensExpr::ParensExpr(const Expr *expr) : expr(expr) {}

ParensExpr::~ParensExpr() { delete expr; }

void ParensExpr::print() const {
  putchar('(');
  expr->print();
  putchar(')');
}

std::vector<ASTNode *> ParensExpr::children() { return {(ASTNode *)expr}; }

bool ParensExpr::operator==(const ASTNode &node) const {
  if (node.get_kind() != ParensExpr::kind) {
    return false;
  }

  const ParensExpr& n = (ParensExpr&) node;

  return (*expr == *(n.expr));
}

ParensTypename::ParensTypename(const Typename *name) : name(name) {}

ParensTypename::~ParensTypename() { delete name; }

void ParensTypename::print() const {
  putchar('(');
  name->print();
  putchar(')');
}

std::vector<ASTNode *> ParensTypename::children() { return {(ASTNode *)name}; }

bool ParensTypename::operator==(const ASTNode &node) const {
  if (node.get_kind() != ParensTypename::kind) {
    return false;
  }

  const ParensTypename& n = (ParensTypename&) node;

  return (*name == *(n.name));
}

PtrTypename::PtrTypename(const Typename *name) : name(name) {}

PtrTypename::~PtrTypename() { delete name; }

void PtrTypename::print() const {
  name->print();
  putchar('*');
}

std::vector<ASTNode *> PtrTypename::children() { return {(ASTNode *)name}; }

bool PtrTypename::operator==(const ASTNode &node) const {
  if (node.get_kind() != PtrTypename::kind) {
    return false;
  }

  const PtrTypename& n = (PtrTypename&) node;

  return (*name == *(n.name));
}

MutTypename::MutTypename(const Typename *name) : name(name) {}

MutTypename::~MutTypename() { delete name; }

void MutTypename::print() const {
  printf("mut ");
  name->print();
}

std::vector<ASTNode *> MutTypename::children() { return {(ASTNode *)name}; }

bool MutTypename::operator==(const ASTNode &node) const {
  if (node.get_kind() != MutTypename::kind) {
    return false;
  }

  const MutTypename& n = (MutTypename&) node;

  return (*name == *(n.name));
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

void TypenameList::push_type(Typename *type_name) {
  types.push_back(type_name);
}

std::vector<ASTNode *> TypenameList::children() { return cast_nodes(types); }

bool TypenameList::operator==(const ASTNode &node) const {
  if (node.get_kind() != TypenameList::kind) {
    return false;
  }

  const TypenameList& n = (TypenameList&) node;

  return cmp_vectors(types, n.types);
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

void VarDeclList::push_decl(VarDecl *decl) { decls.push_back(decl); }

std::vector<ASTNode *> VarDeclList::children() { return cast_nodes(decls); }

bool VarDeclList::operator==(const ASTNode &node) const {
  if (node.get_kind() != VarDeclList::kind) {
    return false;
  }

  const VarDeclList& n = (VarDeclList&) node;

  return cmp_vectors(decls, n.decls);
}

ExprList::ExprList(std::vector<Expr *> exprs) : exprs(exprs) {}

ExprList::~ExprList() { exprs.clear(); }

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

void ExprList::push_expr(Expr *expr) { exprs.push_back(expr); }

std::vector<ASTNode *> ExprList::children() { return cast_nodes(exprs); }

bool ExprList::operator==(const ASTNode &node) const {
  if (node.get_kind() != ExprList::kind) {
    return false;
  }

  const ExprList& n = (ExprList&) node;

  return cmp_vectors(exprs, n.exprs);
}

StatementList::StatementList(std::vector<Statement *> statements)
    : statements(statements) {}

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

void StatementList::push_statement(Statement *statement) {
  statements.push_back(statement);
}

std::vector<ASTNode *> StatementList::children() {
  return cast_nodes(statements);
}

bool StatementList::operator==(const ASTNode &node) const {
  if (node.get_kind() != StatementList::kind) {
    return false;
  }

  const StatementList& n = (StatementList&) node;

  return cmp_vectors(statements, n.statements);
}

TupleTypename::TupleTypename(const TypenameList *type_list)
    : type_list(type_list) {}

TupleTypename::~TupleTypename() { delete type_list; }

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

  const TupleTypename& n = (TupleTypename&) node;

  return (*type_list == *(n.type_list));
}

TupleExpr::TupleExpr(const ExprList *expr_list) : expr_list(expr_list) {}

TupleExpr::~TupleExpr() { delete expr_list; }

void TupleExpr::print() const {
  putchar('[');
  expr_list->print();
  putchar(']');
}

std::vector<ASTNode *> TupleExpr::children() { return {(ASTNode *)expr_list}; }

bool TupleExpr::operator==(const ASTNode &node) const {
  if (node.get_kind() != TupleExpr::kind) {
    return false;
  }

  const TupleExpr& n = (TupleExpr&) node;

  return (*expr_list == *(n.expr_list));
}

FuncTypename::FuncTypename(const TypenameList *params, const Typename *ret_type)
    : params(params), ret_type(ret_type) {}

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

  const FuncTypename& n = (FuncTypename&) node;

  return (*params == *(n.params) && *ret_type == *(n.ret_type));
}

StaticArrayTypename::StaticArrayTypename(const Typename *element_type,
                                         const IntLiteral *size)
    : element_type(element_type), size(size) {}

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

  const StaticArrayTypename& n = (StaticArrayTypename&) node;

  return (*element_type == *(n.element_type) && *size == *(n.size));
}

TypeAlias::TypeAlias(const Ident *name, const Typename *type_expr)
    : name(name), type_expr(type_expr) {}

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

  const TypeAlias& n = (TypeAlias&) node;

  return (*name == *(n.name) && *type_expr == *(n.type_expr));
}

StructDecl::StructDecl(const Ident *name, const VarDeclList *members, const SymbolTable *scope)
    : name(name), members(members), scope(scope) {}

StructDecl::~StructDecl() {
  delete name;
  delete members;
  delete scope;
}

void StructDecl::print() const {
  printf("struct ");
  name->print();
  printf(" {\n");
  members->print();
  printf("};\n");
}

std::vector<ASTNode *> StructDecl::children() {
  return {(Expr *)name, (ASTNode *)members};
}

bool StructDecl::operator==(const ASTNode &node) const {
  if (node.get_kind() != StructDecl::kind) {
    return false;
  }

  const StructDecl& n = (StructDecl&) node;

  return (*name == *(n.name) && *members == *(n.members));
}

VarDecl::VarDecl(const Typename *type_name, const Ident *var_name)
    : type_name(type_name), var_name(var_name) {}

VarDecl::~VarDecl() {
  delete type_name;
  delete var_name;
}

void VarDecl::print() const {
  type_name->print();
  putchar(' ');
  var_name->print();
}

void VarDecl::add_to_scope(SymbolTable *symtable) {
  symtable->put(var_name->id, new Symbol(Var));
}

std::vector<ASTNode *> VarDecl::children() {
  return {(ASTNode *)type_name, (Expr *)var_name};
}

bool VarDecl::operator==(const ASTNode &node) const {
  if (node.get_kind() != VarDecl::kind) {
    return false;
  }

  const VarDecl& n = (VarDecl&) node;

  return (*type_name == *(n.type_name) && *var_name == *(n.var_name));
}

VarDeclInit::VarDeclInit(const VarDecl *decl, const Expr *init)
    : decl(decl), init(init) {}

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

  const VarDeclInit& n = (VarDeclInit&) node;

  return (*decl == *(n.decl) && *init == *(n.init));
}

ArrayLiteral::ArrayLiteral(const ExprList *items) : items(items) {}

ArrayLiteral::~ArrayLiteral() { delete items; }

void ArrayLiteral::print() const {
  putchar('{');
  items->print();
  putchar('}');
}

std::vector<ASTNode *> ArrayLiteral::children() { return {(ASTNode *)items}; }

bool ArrayLiteral::operator==(const ASTNode &node) const {
  if (node.get_kind() != ArrayLiteral::kind) {
    return false;
  }

  const ArrayLiteral& n = (ArrayLiteral&) node;

  return (*items == *(n.items));
}

IfStmt::IfStmt(const Expr *cond, const StatementList *then, const SymbolTable *scope)
    : cond(cond), then(then), scope(scope) {}

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

  const IfStmt& n = (IfStmt&) node;

  return (*cond == *(n.cond) && *then == *(n.then));
}

IfElseStmt::IfElseStmt(const IfStmt *if_stmt, const StatementList *els, const SymbolTable *scope)
    : if_stmt(if_stmt), els(els), scope(scope) {}

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

  const IfElseStmt& n = (IfElseStmt&) node;

  return (*if_stmt == *(n.if_stmt) && *els == *(n.els));
}

WhileStmt::WhileStmt(const Expr *cond, const StatementList *body, const SymbolTable *scope)
    : cond(cond), body(body), scope(scope) {}

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

  const WhileStmt& n = (WhileStmt&) node;

  return (*cond == *(n.cond) && *body == *(n.body));
}

ForStmt::ForStmt(const Statement *init, const Expr *cond,
                 const Statement *update, const StatementList *body, const SymbolTable *scope)
    : init(init), condition(cond), update(update), body(body), scope(scope) {}

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

  const ForStmt& n = (ForStmt&) node;

  return (*init == *(n.init) && *condition == *(n.condition) && *update == *(n.update) && *body == *(n.body));
}

AddrOf::AddrOf(const Expr *expr) : expr(expr) {}

AddrOf::~AddrOf() { delete expr; }

void AddrOf::print() const {
  putchar('&');
  expr->print();
}

std::vector<ASTNode *> AddrOf::children() { return {(ASTNode *)expr}; }

bool AddrOf::operator==(const ASTNode &node) const {
  if (node.get_kind() != AddrOf::kind) {
    return false;
  }

  const AddrOf& n = (AddrOf&) node;

  return (*expr == *(n.expr));
}

Deref::Deref(const Expr *expr) : expr(expr) {}

Deref::~Deref() { delete expr; }

void Deref::print() const {
  putchar('*');
  expr->print();
}

std::vector<ASTNode *> Deref::children() { return {(ASTNode *)expr}; }

bool Deref::operator==(const ASTNode &node) const {
  if (node.get_kind() != Deref::kind) {
    return false;
  }

  const Deref& n = (Deref&) node;

  return (*expr == *(n.expr));
}

CastExpr::CastExpr(const Typename *dest_type, const Expr *expr)
    : dest_type(dest_type), expr(expr) {}

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

  const CastExpr& n = (CastExpr&) node;

  return (*dest_type == *(n.dest_type) && *expr == *(n.expr));
}

LogicalExpr::LogicalExpr(const char *const op, const Expr *l, const Expr *r)
    : op(std::string(op)), left(l), right(r) {}

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

  const LogicalExpr& n = (LogicalExpr&) node;

  return (op == n.op && *left == *(n.left) && *right == *(n.right));
}

FunctionCallExpr::FunctionCallExpr(const CallingExpr *func, const ExprList *args)
    : func(func), args(args) {}

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

  const FunctionCallExpr& n = (FunctionCallExpr&) node;

  return (*func == *(n.func) && *args == *(n.args));
}

FunctionCallStmt::FunctionCallStmt(const CallingExpr *func, const ExprList *args)
    : func(func), args(args) {}

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

  const FunctionCallStmt& n = (FunctionCallStmt&) node;

  return (*func == *(n.func) && *args == *(n.args));
}

ParamsList::ParamsList(std::vector<VarDecl *> params) : params(params) {}

ParamsList::~ParamsList() { params.clear(); }

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

void ParamsList::push_param(VarDecl *param) { params.push_back(param); }

void ParamsList::add_to_scope(SymbolTable *symtable) {
  for (auto &param : params) {
    param->add_to_scope(symtable);
  }
}

std::vector<ASTNode *> ParamsList::children() { return cast_nodes(params); }

bool ParamsList::operator==(const ASTNode &node) const {
  if (node.get_kind() != ParamsList::kind) {
    return false;
  }

  const ParamsList& n = (ParamsList&) node;

  return cmp_vectors(params, n.params);
}

FuncDecl::FuncDecl(const Ident *name, const ParamsList *params,
                   const Typename *ret_type, const StatementList *body, const SymbolTable *scope)
    : name(name), params(params), ret_type(ret_type), body(body), scope(scope) {}

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

  const FuncDecl& n = (FuncDecl&) node;

  return (*name == *(n.name) && *params == *(n.params) && *ret_type == *(n.ret_type) && *body == *(n.body));
}

ProgramSource::ProgramSource(std::vector<ASTNode *> nodes) : nodes(nodes) {}

ProgramSource::~ProgramSource() { nodes.clear(); }

void ProgramSource::print() const {
  for (auto &node : nodes) {
    node->print();
    printf(";\n");
  }
}

void ProgramSource::add_node(ASTNode *node) { nodes.push_back(node); }

std::vector<ASTNode *> ProgramSource::children() { return nodes; }

bool ProgramSource::operator==(const ASTNode &node) const {
  if (node.get_kind() != ProgramSource::kind) {
    return false;
  }

  const ProgramSource& n = (ProgramSource&) node;

  return cmp_vectors(nodes, n.nodes);
}

ReturnStatement::ReturnStatement(const Expr *val) : val(val) {}

ReturnStatement::~ReturnStatement() { delete val; }

void ReturnStatement::print() const {
  printf("return ");
  val->print();
}

std::vector<ASTNode *> ReturnStatement::children() { return {(ASTNode *)val}; }

bool ReturnStatement::operator==(const ASTNode &node) const {
  if (node.get_kind() != ReturnStatement::kind) {
    return false;
  }

  const ReturnStatement& n = (ReturnStatement&) node;

  return (*val == *(n.val));
}

Assignment::Assignment(const Expr *lhs, const Expr *rhs) : lhs(lhs), rhs(rhs) {}

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

  const Assignment& n = (Assignment&) node;

  return (*lhs == *(n.lhs) && *rhs == *(n.rhs));
}

BangExpr::BangExpr(const Expr *expr) : expr(expr) {}

BangExpr::~BangExpr() { delete expr; }

void BangExpr::print() const {
  putchar('!');
  expr->print();
}

std::vector<ASTNode *> BangExpr::children() { return {(ASTNode *)expr}; }

bool BangExpr::operator==(const ASTNode &node) const {
  if (node.get_kind() != BangExpr::kind) {
    return false;
  }

  const BangExpr& n = (BangExpr&) node;

  return (*expr == *(n.expr));
}

NotExpr::NotExpr(const Expr *expr) : expr(expr) {}

NotExpr::~NotExpr() { delete expr; }

void NotExpr::print() const {
  printf("not ");
  expr->print();
}

std::vector<ASTNode *> NotExpr::children() { return {(ASTNode *)expr}; }

bool NotExpr::operator==(const ASTNode &node) const {
  if (node.get_kind() != NotExpr::kind) {
    return false;
  }

  const NotExpr& n = (NotExpr&) node;

  return (*expr == *(n.expr));
}

PreExpr::PreExpr(const char *const op, const Expr *expr)
    : op(std::string(op)), expr(expr) {}

PreExpr::~PreExpr() { delete expr; }

void PreExpr::print() const {
  std::cout << op;
  expr->print();
}

std::vector<ASTNode *> PreExpr::children() { return {(ASTNode *)expr}; }

bool PreExpr::operator==(const ASTNode &node) const {
  if (node.get_kind() != PreExpr::kind) {
    return false;
  }

  const PreExpr& n = (PreExpr&) node;

  return (op == n.op && *expr == *(n.expr));
}
