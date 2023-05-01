#include <stdio.h>

#include "ast.h"
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
        fprintf(stderr, "Undeclared type: %s\n", ident->id.c_str());
        // TODO: Don't exit
        exit(1);
    }

    if (symbol->kind != Type) {
        fprintf(stderr, "Symbol is not a type: %s\n", ident->id.c_str());
        // TODO: Don't exit
        exit(1);
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

        // Order of the members matters because it affects the padding. We probably
        // won't pad structs in this compiler but we should keep the option open
        if (*(this_decl->var_name) != *(t_decl->var_name)) {
            return false;
        }

        if (!this_decl->type_name->type_equals(t_decl->type_name, symtable)) {
            return false;
        }
    }

    return true;
}
