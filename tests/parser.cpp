#include <stdlib.h>

#include "utils.h"
#include "../src/ast.h"
#include "../src/parseutils.h"
#include "../src/symtable.h"

void parser_tests() {
    xtest::tests["struct parse tree"] = []() {
        const char * code = R"(
            struct Point {
                int x;
                int y;
            };
        )";

        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;

        // This symbol table is not accurate; we are only testing syntax here though
        SymbolTable * test_symtable = x::default_symtable();
        Location loc(0, 0, 0, 0);
        Ident * Point = new Ident(loc, "Point");
        VarDecl * x = new VarDecl(loc, new TypeIdent(loc, "int"), new Ident(loc, "x"));
        VarDecl * y = new VarDecl(loc, new TypeIdent(loc, "int"), new Ident(loc, "y"));
        std::vector<VarDecl *> decls = {
            x, y
        };
        VarDeclList * varss = new VarDeclList(loc, decls);
        StructTypename * struct_type = new StructTypename(loc, varss, test_symtable);
        StructDecl * strukt = new StructDecl(loc, Point, struct_type);
        StructDecl * parsed_strukt = (StructDecl *) output->find([](const ASTNode * node) {
            return node->get_kind() == StructDecl::kind;
        });

        expect(*strukt == *parsed_strukt);

        delete strukt;

        return TEST_SUCCESS;
    };
}