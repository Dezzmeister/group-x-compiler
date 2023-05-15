#ifndef SRC_ASM_UTILS_H
#define SRC_ASM_UTILS_H

#include "ast.h"
#include "tac.h"

/**
 * This class owns all Typename ptrs, so be sure to call clone() before inserting
 * a typename into the map
 */
class TypeTable {
    public:
        std::map<std::string, Typename *> types;

        TypeTable();

        ~TypeTable();

        void put(std::string name, Typename * typ);

        /**
         * Look up the symbol and get its type, then insert it into the type table with
         * a new name
         */
        void put_from_symbol(std::string name, std::string new_name, SymbolTable * symtable);

        Typename * get(std::string name);

        int arg_offset(const ArgTAC * tac);
};

namespace x {
    void generate_assembly(const ProgramSource * src, SymbolTable * symtable, std::ostream &code);
}

#endif
