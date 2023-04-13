#include <string.h>
#include "ast.h"

Int::Int(const int value) : value(value) {}

Float::Float(const float value) : value(value) {}

String::String(const char * value) {
    this->value = strdup(value);
}

Print::Print(Expr *e) {
    this->expr = e;
} 

If::If(Expr *e, Stmt *then, Stmt * els) {
    this->expr = e;
    this->then= then;
    this->els = els;
}

If::If(Expr * e, Stmt* then) {
    this->expr = e;
    this->then = then;
}

While::While(Expr *e, Stmt *s) {
    this->expr = e;
    this->stmt = s;
}

Return::Return(Expr * e) {
    this->expr = e;
}

Function::Function(int returnType, Stmt * params, Stmt *stmts) {
    this->returnType = returnType;
    this->params = params;
    this->stmts = stmts;
}

Var::Var(const char * name) {
    this->name = name;
}

VarDecl::VarDecl(int type, Var * v) {
    this->type = type;
    this->v = v;
}

ArithExpr::ArithExpr(char op, Expr * l, Expr * r) {
    this->op = op;
    this->left = l;
    this->right = r;
}

Statements::Statements(Stmt * s1, Stmt * s2) {
    this->left = s1;
    this->right = s2;
}

Paren::Paren(Expr * e) {
    this->expr = e;
}

LogicalExpr::LogicalExpr(const char * op, Expr * left, Expr * right) :
op(op), left(left), right(right) {}

CompExpr::CompExpr(const char * op, Expr * left, Expr * right) :
op(op), left(left), right(right) {}

BitwiseExpr::BitwiseExpr(const char * op, Expr * left, Expr * right) :
op(op), left(left), right(right) {}

UnaryExpr::UnaryExpr(const char * op, Expr * e) :
op(op), expr(e) {}

TernaryExpr::TernaryExpr(Expr * cond, Expr * t, Expr * f) : 
condition(cond), true_expr(t), false_expr(f) {}

