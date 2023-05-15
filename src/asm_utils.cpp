#include "asm_utils.h"

TypeTable::TypeTable() : types({}) {}

TypeTable::~TypeTable() {}

void TypeTable::put(std::string name, TypeEntry entry) {
    types[name] = entry;
}

/**
 * Look up the symbol and get its type, then insert it into the type table with
 * a new name
 */
void TypeTable::put_from_symbol(std::string name, std::string new_name, SymbolTable * symtable) {
    if (has(new_name)) {
        return;
    }
    Symbol * sym = symtable->get(name);
    if (sym == nullptr) {
        fprintf(stderr, "symbol '%s' does not exist\n", name.c_str());
        return;
    }
    if (sym->kind == Type) {
        fprintf(stderr, "tried to insert typename into type table\n");
        return;
    }
    if (sym->kind == Var) {
        VarDecl * decl = sym->decl.var;
        Typename * typ = decl->type_name->clone();
        types[new_name] = {
            .typ = typ,
            .size = typ->type_size(symtable)
        };
        return;
    }
    FuncDecl * decl = sym->decl.func;
    Typename * typ = decl->type_of(symtable);
    int size = typ->type_size(symtable);
    types[new_name] = {
        .typ = typ,
        .size = size
    };
}

TypeEntry TypeTable::get(std::string name) {
    if (types.count(name)) {
        return types[name];
    }
    // TODO: optional
    return {0};
}

bool TypeTable::has(std::string name) {
    return types.count(name);
}

int TypeTable::arg_offset(const ArgTAC * tac) {
    TypeEntry entry = this->get(tac->func);
    Typename * typ = entry.typ;
    if (typ == nullptr) {
        fprintf(stderr, "type of func can't be null\n");
        exit(1);
    }
    if (typ->get_kind() != FuncTypename::kind) {
        fprintf(stderr, "type of func must be func\n");
        exit(1);
    }
    FuncTypename * func_type = (FuncTypename *) typ;
    return func_type->offsets[tac->arg];
}

void x::generate_assembly(const ProgramSource * src, SymbolTable * symtable, std::ostream &code) {
    std::vector<Quad *> instrs = {};
    TypeTable type_table;
    NamesToNames names = x::symtable_to_names(nullptr, symtable);
    src->gen_tac(symtable, &type_table, names, instrs);

    AsmState asm_state;
    code << ".text\n";
    code << ".globl main\n";
    
    for (auto &tac : instrs) {
        tac->to_asm(code, &type_table, names, asm_state);
    }

    code << std::flush;
}
