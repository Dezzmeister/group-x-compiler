#include "asm_utils.h"

TypeTable::TypeTable() : types({}) {}

TypeTable::~TypeTable() {
    for (std::pair<const std::string, Typename *> &item : types) {
        delete item.second;
    }
}

void TypeTable::put(std::string name, Typename * typ) {
    types[name] = typ;
}

/**
 * Look up the symbol and get its type, then insert it into the type table with
 * a new name
 */
void TypeTable::put_from_symbol(std::string name, std::string new_name, SymbolTable * symtable) {
    if (get(new_name) != nullptr) {
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
        types[new_name] = typ;
        return;
    }
    FuncDecl * decl = sym->decl.func;
    Typename * typ = decl->type_of(symtable);
    types[new_name] = typ;
}

Typename * TypeTable::get(std::string name) {
    if (types.count(name)) {
        return types[name];
    }
    return nullptr;
}

int TypeTable::arg_offset(const ArgTAC * tac) {
    Typename * typ = this->get(tac->func);
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