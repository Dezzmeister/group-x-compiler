#include <algorithm>
#include <memory>
#include <sstream>
#include <stdio.h>

#include "ast.h"
#include "errors.h"
#include "symtable.h"

/**
 * Returns a type which is semantically equivalent to typ, but is not an alias or a
 * typename in parens.
 */
static const Typename * unaliased(const Typename * typ, SymbolTable * symtable) {
    if (typ->get_kind() == ParensTypename::kind) {
        const ParensTypename * p = (ParensTypename *) typ;

        return unaliased(p->name, symtable);
    }

    if (typ->get_kind() != TypeIdent::kind) {
        return typ;
    }

    const TypeIdent * ident = (TypeIdent *) typ;

    const Symbol * symbol = symtable->get(ident->id);

    if (!symbol) {
        __builtin_unreachable();
    }

    if (symbol->kind != Type) {
        __builtin_unreachable();
    }

    if (symbol->decl.typ == nullptr) {
        // Primitive type
        return typ;
    }

    if (symbol->decl.typ->get_kind() == TypeAlias::kind) {
        const TypeAlias * alias = (TypeAlias *) symbol->decl.typ;

        return unaliased(alias->type_expr, symtable);
    } else if (symbol->decl.typ->get_kind() == StructDecl::kind) {
        const StructDecl * decl = (StructDecl *) symbol->decl.typ;

        return decl->defn;
    }

    return typ;
}

static const Typename * base_type(const Typename * typ, SymbolTable * symtable) {
    if (typ->get_kind() == MutTypename::kind) {
        const MutTypename * t_mut = (MutTypename *) typ;

        return unaliased(t_mut->name, symtable);
    }

    return typ;
}

bool ParensTypename::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * name_unalias = unaliased(name, symtable);

    return name_unalias->type_equals(unaliased(t, symtable), symtable);
}

bool TypeIdent::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * this_unalias = unaliased(this, symtable);

    if (this_unalias != this) {
        return this_unalias->type_equals(unaliased(t, symtable), symtable);
    }

    return *this == *(unaliased(t, symtable));
}

bool PtrTypename::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    if (t_unalias->get_kind() != PtrTypename::kind) {
        return false;
    }

    const PtrTypename * t_ptr = (PtrTypename *) t_unalias;

    return name->type_equals(t_ptr->name, symtable);
}

bool MutTypename::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    if (t_unalias->get_kind() != MutTypename::kind) {
        return false;
    }

    const MutTypename * t_mut = (MutTypename *) t_unalias;

    return name->type_equals(t_mut->name, symtable);
}

bool TupleTypename::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    if (t_unalias->get_kind() != TupleTypename::kind) {
        return false;
    }

    const TupleTypename * t_tuple = (TupleTypename *) t_unalias;

    if (type_list->types.size() != t_tuple->type_list->types.size()) {
        return false;
    }

    for (size_t i = 0; i < type_list->types.size(); i++) {
        if (!type_list->types[i]->type_equals(t_tuple->type_list->types[i], symtable)) {
            return false;
        }
    }

    return true;
}

bool FuncTypename::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    if (t_unalias->get_kind() != FuncTypename::kind) {
        return false;
    }

    const FuncTypename * t_func = (FuncTypename *) t_unalias;

    if (!ret_type->type_equals(t_func->ret_type, symtable)) {
        return false;
    }

    if (params->types.size() != t_func->params->types.size()) {
        return false;
    }

    for (size_t i = 0; i < params->types.size(); i++) {
        if (!params->types[i]->type_equals(t_func->params->types[i], symtable)) {
            return false;
        }
    }

    return true;
}

bool StaticArrayTypename::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    if (t_unalias->get_kind() != StaticArrayTypename::kind) {
        return false;
    }

    const StaticArrayTypename * t_array = (StaticArrayTypename *) t_unalias;

    return (element_type->type_equals(t_array->element_type, symtable) && *size == *(t_array->size));
}

bool DynamicArrayTypename::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);
    if (t_unalias->get_kind() != DynamicArrayTypename::kind) {
        return false;
    }

    const DynamicArrayTypename * t_array = (DynamicArrayTypename *) t_unalias;

    return (element_type->type_equals(t_array->element_type, symtable));
}

bool DynamicArrayTypename::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);
    if(t_unalias->get_kind() == PtrTypename::kind) {
        PtrTypename *t_ptr = (PtrTypename *) t_unalias;
        return (this->element_type->type_equals(t_ptr->name, symtable));
    }
    return false;
}

int DynamicArrayTypename::type_size(SymbolTable * symtable) const {
    return ADDRESS_WIDTH;
}

bool StructTypename::type_equals(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    if (t_unalias->get_kind() != StructTypename::kind) {
        return false;
    }

    const StructTypename * t_struct = (StructTypename *) t_unalias;

    if (members->decls.size() != t_struct->members->decls.size()) {
        return false;
    }

    for (size_t i = 0; i < members->decls.size(); i++) {
        const VarDecl * this_decl = members->decls[i];
        const VarDecl * t_decl = t_struct->members->decls[i];
        const Typename * this_decl_type = unaliased(this_decl->type_name, symtable);
        const Typename * t_decl_type = unaliased(t_decl->type_name, symtable);

        // Order of the members matters because it affects the padding. We probably
        // won't pad structs in this compiler but we should keep the option open
        if (*(this_decl->var_name) != *(t_decl->var_name)) {
            return false;
        }

        if (!this_decl_type->type_equals(t_decl_type, symtable)) {
            return false;
        }
    }

    return true;
}

bool ParensTypename::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    const Typename * this_unalias = unaliased(name, symtable);
    const Typename * t_unalias = unaliased(t, symtable);

    return this_unalias->can_cast_to(t_unalias, symtable);
}

bool TypeIdent::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    const Typename * this_unalias = unaliased(this, symtable);
    const Typename * t_unalias = unaliased(t, symtable);

    if (this_unalias != this) {
        return this_unalias->can_cast_to(t_unalias, symtable);
    }

    TypeIdent int_type(x::NULL_LOC, "int");
    TypeIdent float_type(x::NULL_LOC, "float");

    // An int can be cast to a ptr to anything
    if (t_unalias->get_kind() == PtrTypename::kind && this->type_equals(&int_type, symtable)) {
        return true;
    }

    if (t_unalias->get_kind() != TypeIdent::kind) {
        return false;
    }

    // At this point we're dealing with two primitive types - a TypeIdent that is
    // not an alias is a primitive type

    if (this->type_equals(t_unalias, symtable)) {
        return true;
    }

    return (this->type_equals(&int_type, symtable) && t_unalias->type_equals(&float_type, symtable));
}

bool PtrTypename::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    if (name->type_equals(t_unalias, symtable)) {
        return true;
    }

    TypeIdent int_type(x::NULL_LOC, "int");

    if (t_unalias->type_equals(&int_type, symtable)) {
        return true;
    }

    if (t_unalias->get_kind() != PtrTypename::kind) {
        return false;
    }

    const PtrTypename * t_ptr = (PtrTypename *) t_unalias;

    return (name->type_equals(t_ptr->name, symtable));
}

bool MutTypename::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    if (t_unalias->get_kind() != MutTypename::kind) {
        return false;
    }

    const MutTypename * t_mut = (MutTypename *) t_unalias;

    return name->can_cast_to(t_mut->name, symtable);
}

bool TupleTypename::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    // [int] can cast to int
    if (type_list->types.size() == 1 && type_list->types[0]->type_equals(t_unalias, symtable)) {
        return true;
    }

    if (t_unalias->get_kind() != TupleTypename::kind) {
        return false;
    }

    const TupleTypename * t_tuple = (TupleTypename *) t_unalias;

    if (type_list->types.size() != t_tuple->type_list->types.size()) {
        return false;
    }

    for (size_t i = 0; i < type_list->types.size(); i++) {
        if (!type_list->types[i]->type_equals(t_tuple->type_list->types[i], symtable)) {
            return false;
        }
    }

    return true;
}

bool FuncTypename::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    return this->type_equals(t, symtable);
}

bool StaticArrayTypename::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    const Typename * t_unalias = unaliased(t, symtable);

    // int[1] can cast to int
    if (size->value == 1 && element_type->type_equals(t_unalias, symtable)) {
        return true;
    }

    if (t_unalias->get_kind() != StaticArrayTypename::kind) {
        return false;
    }

    const StaticArrayTypename * t_array = (StaticArrayTypename *) t_unalias;

    return (*size == *(t_array->size) && element_type->can_cast_to(t_array->element_type, symtable));
}

bool StructTypename::can_cast_to(const Typename * t, SymbolTable * symtable) const {
    return this->type_equals(t, symtable);
}

Typename * ParensExpr::type_of(SymbolTable * symtable) const {
    return expr->type_of(symtable);
}

Typename * IntLiteral::type_of(SymbolTable * symtable) const {
    return new TypeIdent(x::NULL_LOC, "int");
}

Typename * FloatLiteral::type_of(SymbolTable * symtable) const {
    return new TypeIdent(x::NULL_LOC, "float");
}

Typename * TernaryExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> cond_type(cond->type_of(symtable));
    std::unique_ptr<Typename> tru_type(tru->type_of(symtable));
    std::unique_ptr<Typename> fals_type(fals->type_of(symtable));

    TypeIdent bool_type(x::NULL_LOC, "bool");

    if (!cond_type->type_equals(&bool_type, symtable)) {
        // TODO: Print type
        throw CompilerError(cond->loc, "Condition in ternary expression must have bool type", Error);
    }

    // TODO: Implicit cast if possible
    if (!tru_type->type_equals(fals_type.get(), symtable)) {
        // TODO: Print type
        throw CompilerError(Location(tru->loc, fals->loc), "Both sides of ternary expression must have the same type", Error);
    }

    return tru_type.release();
}

Typename * BoolLiteral::type_of(SymbolTable * symtable) const {
    return new TypeIdent(x::NULL_LOC, "bool");
}

Typename * CharLiteral::type_of(SymbolTable * symtable) const {
    return new TypeIdent(x::NULL_LOC, "char");
}

Typename * StringLiteral::type_of(SymbolTable * symtable) const {
    // Same as in C, a string literal has type "char *"
    return new PtrTypename(x::NULL_LOC, new TypeIdent(x::NULL_LOC, "char"));
}

Typename * Ident::type_of(SymbolTable * symtable) const {
    Symbol * symbol = symtable->get(id);

    if (symbol->kind == Type) {
        throw CompilerError(loc, "Unexpected type identifier", Error);
    }

    if (symbol->kind == Var) {
        VarDecl * decl = symbol->decl.var;

        return unaliased(decl->type_name, symtable)->clone();
    }

    FuncDecl * decl = symbol->decl.func;

    std::vector<Typename *> params_clone = {};

    for (auto &param : decl->params->params) {
        Typename * type_name = param->type_name->clone();

        params_clone.push_back(type_name);
    }

    return new FuncTypename(x::NULL_LOC, new TypenameList(x::NULL_LOC, params_clone), decl->ret_type->clone());
}

Typename * MathExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> lhs_type(left->type_of(symtable));
    std::unique_ptr<Typename> rhs_type(right->type_of(symtable));

    // TODO: Make these constant and reference them throughout the program
    const TypeIdent int_type(x::NULL_LOC, "int");
    const TypeIdent float_type(x::NULL_LOC, "float");

    const Typename * lhs_base = base_type(lhs_type.get(), symtable);
    const Typename * rhs_base = base_type(rhs_type.get(), symtable);

    // TODO: Use '==' overload here, these can only be simple TypeIdents
    // TODO: More sophisticated typechecking here when we add different sizes of ints and floats
    const bool l_is_int = lhs_base->type_equals(&int_type, symtable);
    const bool l_is_float = lhs_base->type_equals(&float_type, symtable);
    const bool r_is_int = rhs_base->type_equals(&int_type, symtable);
    const bool r_is_float = rhs_base->type_equals(&float_type, symtable);

    if (!(l_is_int || l_is_float)) {
        throw CompilerError(left->loc, "Expected int or float", Error);
    }

    if (!(r_is_int || r_is_float)) {
        throw CompilerError(right->loc, "Expected int or float", Error);
    }

    if (op == '%' && (l_is_float || r_is_float)) {
        throw CompilerError(loc, "Cannot apply modulo to a float", Error);
    }

    if (l_is_int && r_is_int) {
        return new TypeIdent(x::NULL_LOC, "int");
    }

    // Implicitly promote ints to floats? As of right now this doesn't break our type system
    // principle of casting because we go from a 32 bit int to a 64 bit float, and a 64 bit
    // float can represent every 32 bit int because the fraction is 52 bits
    return new TypeIdent(x::NULL_LOC, "float");
}

Typename * BoolExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> lhs_type(left->type_of(symtable));
    std::unique_ptr<Typename> rhs_type(right->type_of(symtable));

    const TypeIdent bool_type(x::NULL_LOC, "bool");

    const Typename * lhs_base = base_type(lhs_type.get(), symtable);
    const Typename * rhs_base = base_type(rhs_type.get(), symtable);

    if (!lhs_base->type_equals(&bool_type, symtable)) {
        throw CompilerError(left->loc, "Expected bool", Error);
    }

    if (!rhs_base->type_equals(&bool_type, symtable)) {
        throw CompilerError(right->loc, "Expected bool", Error);
    }

    return new TypeIdent(x::NULL_LOC, "bool");
}

Typename * FunctionCallExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> caller_type(func->type_of(symtable));

    if (caller_type->get_kind() != FuncTypename::kind) {
        throw CompilerError(func->loc, "Expression is not callable", Error);
    }

    const FuncTypename * caller_func = (FuncTypename *) caller_type.get();

    if (args->exprs.size() != caller_func->params->types.size()) {
        std::ostringstream stream;
        stream << "Expected " << caller_func->params->types.size() << " args, got " << args->exprs.size();
        throw CompilerError(args->loc, stream.str(), Error);
    }

    for (size_t i = 0; i < args->exprs.size(); i++) {
        const std::unique_ptr<Typename> actual_type(args->exprs[i]->type_of(symtable));
        const Typename * expected_type = caller_func->params->types[i];
        const Typename * actual_base = base_type(actual_type.get(), symtable);

        if (!actual_base->type_equals(expected_type, symtable)) {
            throw CompilerError(args->exprs[i]->loc, "Type mismatch", Error);
        }
    }

    return caller_func->ret_type->clone();
}

Typename * TupleExpr::type_of(SymbolTable * symtable) const {
    std::vector<std::unique_ptr<Typename>> tuple_types(expr_list->exprs.size());
    std::vector<Typename *> raw_tuple_types = {};

    for (size_t i = 0; i < expr_list->exprs.size(); i++) {
        tuple_types[i] = std::unique_ptr<Typename>(expr_list->exprs[i]->type_of(symtable));
    }

    std::transform(tuple_types.begin(), tuple_types.end(), std::back_inserter(raw_tuple_types),
    [](auto &item) {
        return item.release();
    });

    return new TupleTypename(x::NULL_LOC, new TypenameList(x::NULL_LOC, raw_tuple_types), symtable);
}

Typename * ArrayLiteral::type_of(SymbolTable * symtable) const {
    if (items->exprs.size() == 0) {
        throw CompilerError(loc, "Empty array literals are not allowed", Error);
    }

    std::unique_ptr<Typename> out(items->exprs[0]->type_of(symtable));

    for (size_t i = 1; i < items->exprs.size(); i++) {
        std::unique_ptr<Typename> expr_type(items->exprs[i]->type_of(symtable));

        if (!out->type_equals(expr_type.get(), symtable)) {
            throw CompilerError(items->exprs[i]->loc, "All exprs in array literal must have the same type", Error);
        }
    }

    return new StaticArrayTypename(x::NULL_LOC, out.release(), new IntLiteral(x::NULL_LOC, items->exprs.size()));
}

// TODO: rvalue references
Typename * AddrOf::type_of(SymbolTable * symtable) const {
    return new PtrTypename(x::NULL_LOC, expr->type_of(symtable));
}

Typename * Deref::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> expr_type(expr->type_of(symtable));

    if (expr_type->get_kind() != PtrTypename::kind) {
        throw CompilerError(expr->loc, "Cannot dereference a non-pointer type", Error);
    }

    const PtrTypename * ptr_type = (PtrTypename *) expr_type.get();
    const Typename * base_type = unaliased(ptr_type->name, symtable);

    return base_type->clone();
}

Typename * CastExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> expr_type(expr->type_of(symtable));

    if (!expr_type->can_cast_to(dest_type, symtable)) {
        throw CompilerError(loc, "Cast is invalid: expr type cannot be cast to destination type", Error);
    }

    return dest_type->clone();
}

Typename * LogicalExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> lhs_type(left->type_of(symtable));
    std::unique_ptr<Typename> rhs_type(right->type_of(symtable));

    TypeIdent int_type(x::NULL_LOC, "int");
    TypeIdent float_type(x::NULL_LOC, "float");

    const Typename * lhs_base = base_type(lhs_type.get(), symtable);
    const Typename * rhs_base = base_type(rhs_type.get(), symtable);

    if (op == std::string("==") || op == std::string("!=")) {
        if (!lhs_base->type_equals(rhs_base, symtable)) {
            throw CompilerError(loc, "Both sides of equality comparison must have the same type", Error);
        }

        return new TypeIdent(x::NULL_LOC, "bool");
    }

    if (op == std::string("in") || op == std::string("not in")) {
        if (rhs_base->get_kind() != StaticArrayTypename::kind) {
            throw CompilerError(right->loc, "'in' and 'not in' can only be used to check for presence in an array", Error);
        }

        std::unique_ptr<StaticArrayTypename> rhs_array((StaticArrayTypename *) rhs_type.release());

        if (!lhs_base->type_equals(rhs_array->element_type, symtable)) {
            throw CompilerError(left->loc, "Type mismatch: left hand side is not an element of right hand side", Error);
        }

        return new TypeIdent(x::NULL_LOC, "bool");
    }

    // Now the operation can only be one of the relational numeric operators, so the operands
    // must be numeric

    const bool lhs_is_num = lhs_base->type_equals(&int_type, symtable) || lhs_base->type_equals(&float_type, symtable);
    const bool rhs_is_num = rhs_base->type_equals(&int_type, symtable) || rhs_base->type_equals(&float_type, symtable);

    if (!lhs_is_num) {
        throw CompilerError(left->loc, "Left hand side must be numeric type", Error);
    }

    if (!rhs_is_num) {
        throw CompilerError(right->loc, "Right hand side must be numeric type", Error);
    }

    return new TypeIdent(x::NULL_LOC, "bool");
}

Typename * FuncDecl::type_of(SymbolTable * symtable) const {
    std::vector<Typename *> param_types = {};

    for (size_t i = 0; i < params->params.size(); i++) {
        param_types.push_back(params->params[i]->type_name->clone());
    }

    return new FuncTypename(x::NULL_LOC, new TypenameList(x::NULL_LOC, param_types), ret_type->clone());
}

Typename * BangExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> expr_type(expr->type_of(symtable));

    TypeIdent bool_type(x::NULL_LOC, "bool");

    if (!expr_type->type_equals(&bool_type, symtable)) {
        throw CompilerError(loc, "Bang can only be used on bool expr", Error);
    }

    return new TypeIdent(x::NULL_LOC, "bool");
}

Typename * NotExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> expr_type(expr->type_of(symtable));

    TypeIdent bool_type(x::NULL_LOC, "bool");

    if (!expr_type->type_equals(&bool_type, symtable)) {
        throw CompilerError(loc, "'not' can only be used on bool expr", Error);
    }

    return new TypeIdent(x::NULL_LOC, "bool");
}

Typename * PostExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> expr_type(expr->type_of(symtable));

    TypeIdent int_type(x::NULL_LOC, "int");

    if (!expr_type->type_equals(&int_type, symtable)) {
        throw CompilerError(loc, "Post expr can only be used on int expr", Error);
    }

    return new TypeIdent(x::NULL_LOC, "int");
}

Typename * PreExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> expr_type(expr->type_of(symtable));

    TypeIdent int_type(x::NULL_LOC, "int");

    if (!expr_type->type_equals(&int_type, symtable)) {
        throw CompilerError(loc, "Pre expr can only be used on int expr", Error);
    }

    return new TypeIdent(x::NULL_LOC, "int");
}

Typename * StructDeref::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> strukt_type(strukt->type_of(symtable));

    if (strukt_type->get_kind() != StructTypename::kind) {
        throw CompilerError(strukt->loc, "Left hand side must be a struct type", Error);
    }

    const StructTypename * strukt_struct = (StructTypename *) strukt_type.get();
    SymbolTable * strukt_scope = strukt_struct->scope;

    const Symbol * sym = strukt_scope->get(member->id);

    if (sym == nullptr) {
        throw CompilerError(member->loc, "Right hand side must be a member of left hand side struct", Error);
    }

    if (sym->kind != Var) {
        throw CompilerError(member->loc, "Right hand side must be a variable member of left hand side struct", Error);
    }

    const VarDecl * decl = sym->decl.var;

    return decl->type_name->clone();
}

Typename * StructLiteral::type_of(SymbolTable * symtable) const {
    std::vector<std::unique_ptr<VarDecl>> var_decls = {};
    std::unique_ptr<SymbolTable> scope(new SymbolTable(symtable));

    for (auto &member : members->members) {
        std::unique_ptr<Typename> expr_type(member->expr->type_of(symtable));
        const Typename * var_type = expr_type.release();
        const Ident * var_name = new Ident(x::NULL_LOC, member->member->id.c_str());
        VarDecl * var_decl = new VarDecl(x::NULL_LOC, var_type, var_name);

        scope->put(var_decl->var_name->id, new Symbol(Var, (Decl) {
            .var = var_decl
        }));
        var_decls.push_back(std::unique_ptr<VarDecl>(var_decl));
    }

    std::vector<VarDecl *> var_decls_raw = {};

    for (auto &var_decl : var_decls) {
        var_decls_raw.push_back(var_decl.release());
    }

    const VarDeclList * decl_list = new VarDeclList(x::NULL_LOC, var_decls_raw);
    return new StructTypename(x::NULL_LOC, decl_list, scope.release());
}

Typename * ArrayIndexExpr::type_of(SymbolTable * symtable) const {
    std::unique_ptr<Typename> arr_type(arr->type_of(symtable));
    std::unique_ptr<Typename> index_type(index->type_of(symtable));
    const Typename * index_base = base_type(unaliased(index_type.get(), symtable), symtable);

    if (arr_type->get_kind() != StaticArrayTypename::kind) {
        throw CompilerError(x::NULL_LOC, "Left hand side of array index expr is not array", Error);
    }

    TypeIdent int_type(x::NULL_LOC, "int");

    if (!index_base->type_equals(&int_type, symtable)) {
        throw CompilerError(x::NULL_LOC, "Array index type must be int", Error);
    }

    const StaticArrayTypename * static_arr_type = (StaticArrayTypename *) arr_type.get();

    return static_arr_type->element_type->clone();
}

void ProgramSource::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    for (auto &node : nodes) {
        try {
            if (node->get_kind() == VarDecl::kind) {
                const VarDecl * decl = (VarDecl *) node;

                decl->typecheck(symtable, errors);
            } else if (node->get_kind() == VarDeclInit::kind) {
                const VarDeclInit * decl = (VarDeclInit *) node;

                decl->typecheck(symtable, errors);
            } else if (node->get_kind() == FuncDecl::kind) {
                const FuncDecl * decl = (FuncDecl *) node;

                decl->typecheck(symtable, errors);
            }
        } catch (CompilerError error) {
            errors.type_errors.push_back(error);
        }
    }
}

void StatementList::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    for (auto &stmt : statements) {
        try {
            stmt->typecheck(symtable, errors);
        } catch (CompilerError error) {
            errors.type_errors.push_back(error);
        }
    }

    for (auto &item : symtable->table) {
        Symbol * sym = item.second;

        if (!sym->initialized) {
            CompilerError error(sym->decl_upcast()->loc, "Symbol declared but never initialized", Warn);
            errors.type_errors.push_back(error);
        }
    }
}

void VarDecl::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    std::unique_ptr<Typename> expr_type(var_name->type_of(symtable));

    if (!expr_type->type_equals(type_name, symtable)) {
        throw CompilerError(loc, "Type mismatch: left hand side type must match right hand side type", Error);
    }
}

void FunctionCallStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    std::unique_ptr<Typename> caller_type(func->type_of(symtable));

    if (caller_type->get_kind() != FuncTypename::kind) {
        throw CompilerError(func->loc, "Expression is not callable", Error);
    }

    const FuncTypename * caller_func = (FuncTypename *) caller_type.get();

    if (args->exprs.size() != caller_func->params->types.size()) {
        std::ostringstream stream;
        stream << "Expected " << caller_func->params->types.size() << " arg(s), got " << args->exprs.size();
        throw CompilerError(args->loc, stream.str(), Error);
    }

    for (size_t i = 0; i < args->exprs.size(); i++) {
        const std::unique_ptr<Typename> actual_type(args->exprs[i]->type_of(symtable));
        const Typename * actual_base = base_type(actual_type.get(), symtable);
        const Typename * expected_type = caller_func->params->types[i];

        // You can pass a mut x where a regular x is expected
        if (!actual_base->type_equals(expected_type, symtable)) {
            throw CompilerError(args->exprs[i]->loc, "Type mismatch", Error);
        }
    }
}

void TypeAlias::typecheck(SymbolTable * symtable, SourceErrors &errors) const {

}

void StructDecl::typecheck(SymbolTable * symtable, SourceErrors &errors) const {

}

void VarDeclInit::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    std::unique_ptr<Typename> init_type(init->type_of(symtable));

    const Typename * init_base = base_type(init_type.get(), symtable);
    const Typename * decl_base = base_type(decl->type_name, symtable);

    if (!init_base->type_equals(decl_base, symtable)) {
        throw CompilerError(loc, "Type mismatch: left hand side type must match right hand side type", Error);
    }
}

void IfStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    std::unique_ptr<Typename> cond_type(cond->type_of(symtable));

    TypeIdent bool_type(x::NULL_LOC, "bool");

    if (!cond_type->type_equals(&bool_type, symtable)) {
        CompilerError err(cond->loc, "Condition type must be bool", Error);
        errors.type_errors.push_back(err);
    }

    then->typecheck(scope, errors);
}

void IfElseStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    if_stmt->typecheck(scope, errors);
    els->typecheck(scope, errors);
}

void WhileStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    std::unique_ptr<Typename> cond_type(cond->type_of(symtable));

    TypeIdent bool_type(x::NULL_LOC, "bool");

    if (!cond_type->type_equals(&bool_type, symtable)) {
        CompilerError err(cond->loc, "Condition type must be bool", Error);
        errors.type_errors.push_back(err);
    }

    body->typecheck(scope, errors);
}

void ForStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    init->typecheck(scope, errors);

    std::unique_ptr<Typename> cond_type(condition->type_of(symtable));

    TypeIdent bool_type(x::NULL_LOC, "bool");

    if (!cond_type->type_equals(&bool_type, symtable)) {
        CompilerError err(condition->loc, "Condition type must be bool", Error);
        errors.type_errors.push_back(err);
    }

    update->typecheck(scope, errors);
    body->typecheck(scope, errors);
}

void FuncDecl::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    for (auto &decl : params->params) {
        try {
            decl->typecheck(scope, errors);
        } catch (CompilerError error) {
            errors.type_errors.push_back(error);
        }
    }

    body->typecheck(scope, errors);

    if (forward_decl != nullptr) {
        std::unique_ptr<Typename> func_type(this->type_of(symtable));

        if (!func_type->type_equals(forward_decl->type_name, symtable)) {
            CompilerError error(x::NULL_LOC, "Forward declaration type does not match function type", Error);
            errors.type_errors.push_back(error);
        }
    }

    TypeIdent void_type(x::NULL_LOC, "void");
    TypeIdent Please_type(x::NULL_LOC, "Please");

    const size_t num_stmts = body->statements.size();
    const bool ends_with_ret = num_stmts > 0 && body->statements[num_stmts - 1]->get_kind() == ReturnStatement::kind;

    if ((!ret_type->type_equals(&void_type, symtable) && !ends_with_ret) || (!ret_type->type_equals(&Please_type, symtable) && !ends_with_ret)) {
        CompilerError error(x::NULL_LOC, "Missing return statement at end of non-Please/void function", Error);
        errors.type_errors.push_back(error);
    }
}

void ReturnStatement::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    FuncDecl * enclosing_func = nullptr;
    SymbolTable * table = symtable;

    while (table != nullptr) {
        if (table->node == nullptr) {
            CompilerError err(loc, "Return statement must be in function", Error);
            errors.type_errors.push_back(err);
            return;
        }

        if (table->node->get_kind() == FuncDecl::kind) {
            enclosing_func = (FuncDecl *) table->node;
            break;
        }

        table = table->enclosing;
    }

    if (enclosing_func == nullptr) {
        CompilerError err(loc, "Return statement must be in function", Error);
        errors.type_errors.push_back(err);

        try {
            std::unique_ptr<Typename> val_type(val->type_of(symtable));
        } catch (CompilerError error) {
            errors.type_errors.push_back(error);
        }

        return;
    }

    try {
        std::unique_ptr<Typename> val_type(val->type_of(symtable));
        const Typename * ret_type = enclosing_func->ret_type;

        if (!val_type->type_equals(ret_type, symtable)) {
            CompilerError err(val->loc, "Return type does not match expression type", Error);
            errors.type_errors.push_back(err);
        }
    } catch (CompilerError error) {
        errors.type_errors.push_back(error);
    }
}

void Assignment::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    std::unique_ptr<Typename> lhs_type(lhs->type_of(symtable));
    std::unique_ptr<Typename> rhs_type(rhs->type_of(symtable));

    if (lhs_type->get_kind() != MutTypename::kind) {
        throw CompilerError(lhs->loc, "Cannot assign to immutable", Error);
    }

    if (rhs_type->get_kind() == MutTypename::kind) {
        if (!lhs_type->type_equals(rhs_type.get(), symtable)) {
            throw CompilerError(loc, "Left hand side and right hand side types must match", Error);
        }
    } else {
        const MutTypename * lhs_mut = (MutTypename *) lhs_type.get();

        if (!lhs_mut->name->type_equals(rhs_type.get(), symtable)) {
            throw CompilerError(loc, "Left hand side and right hand side types must match", Error);
        }
    }
}

void VoidReturnStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    FuncDecl * enclosing_func = nullptr;
    SymbolTable * table = symtable;

    while (table != nullptr) {
        if (table->node == nullptr) {
            CompilerError err(loc, "Return statement must be in function", Error);
            errors.type_errors.push_back(err);
            return;
        }

        if (table->node->get_kind() == FuncDecl::kind) {
            enclosing_func = (FuncDecl *) table->node;
            break;
        }

        table = table->enclosing;
    }

    if (enclosing_func == nullptr) {
        CompilerError err(loc, "Return statement must be in function", Error);
        errors.type_errors.push_back(err);
        return;
    }

    TypeIdent void_type(x::NULL_LOC, "void");

    if (!enclosing_func->ret_type->type_equals(&void_type, symtable)) {
        CompilerError err(loc, "Cannot return void from non-Please/void function", Error);
        errors.type_errors.push_back(err);
    }
}

void PleaseReturnStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    FuncDecl * enclosing_func = nullptr;
    SymbolTable * table = symtable;

    while (table != nullptr) {
        if (table->node == nullptr) {
            CompilerError err(loc, "Return statement must be in function", Error);
            errors.type_errors.push_back(err);
            return;
        }

        if (table->node->get_kind() == FuncDecl::kind) {
            enclosing_func = (FuncDecl *) table->node;
            break;
        }

        table = table->enclosing;
    }

    if (enclosing_func == nullptr) {
        CompilerError err(loc, "Return statement must be in function", Error);
        errors.type_errors.push_back(err);
        return;
    }

    TypeIdent Please_type(x::NULL_LOC, "Please");

    if (!enclosing_func->ret_type->type_equals(&Please_type, symtable)) {
        CompilerError err(loc, "Cannot return void from non-Please/void function", Error);
        errors.type_errors.push_back(err);
    }
}

void ContinueStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    SymbolTable * table = symtable;

    while (table != nullptr) {
        if (table->node == nullptr) {
            CompilerError error(loc, "Continue statement must occur inside loop", Error);
            errors.type_errors.push_back(error);
            return;
        }

        if (table->node->get_kind() == ForStmt::kind) {
            return;
        } else if (table->node->get_kind() == WhileStmt::kind) {
            return;
        }

        table = table->enclosing;
    }

    CompilerError error(loc, "Continue statement must occur inside loop", Error);
    errors.type_errors.push_back(error);
}

void BreakStmt::typecheck(SymbolTable * symtable, SourceErrors &errors) const {
    SymbolTable * table = symtable;

    while (table != nullptr) {
        if (table->node == nullptr) {
            CompilerError error(loc, "Break statement must occur inside loop", Error);
            errors.type_errors.push_back(error);
            return;
        }

        if (table->node->get_kind() == ForStmt::kind) {
            return;
        } else if (table->node->get_kind() == WhileStmt::kind) {
            return;
        }

        table = table->enclosing;
    }

    CompilerError error(loc, "Break statement must occur inside loop", Error);
    errors.type_errors.push_back(error);
}

int ParensTypename::type_size(SymbolTable * symtable) const {
    return name->type_size(symtable);
}

int TypeIdent::type_size(SymbolTable * symtable) const {
    return unaliased(this, symtable)->type_size(symtable);
}

int PtrTypename::type_size(SymbolTable * symtable) const {
    return ADDRESS_WIDTH;
}

int MutTypename::type_size(SymbolTable * symtable) const {
    return name->type_size(symtable);
}

int TupleTypename::type_size(SymbolTable * symtable) const {
    int size = 0;

    for (auto &item : type_list->types) {
        size += item->type_size(symtable);
    }

    return size;
}

int FuncTypename::type_size(SymbolTable * symtable) const {
    return ADDRESS_WIDTH;
}

int StaticArrayTypename::type_size(SymbolTable * symtable) const {
    return element_type->type_size(symtable) * size->value;
}

int StructTypename::type_size(SymbolTable * symtable) const {
    int size = 0;

    for (auto &decl : members->decls) {
        size += decl->type_name->type_size(symtable);
    }

    return size;
}
