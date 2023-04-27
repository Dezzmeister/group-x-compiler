#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../src/ast.h"
#include "../src/parser.h"

int stub_test() {
    // Print the name of the test. __FUNCTION__ refers to the current function's
    // name
    printf("Running %s\n", __FUNCTION__);
    assert(NUM_LITERAL == 1);

    return EXIT_SUCCESS;
}

int hello_world_test() {
    printf("Running %s\n", __FUNCTION__);
    string str = "Hello World";
    assert(str == "Hello World");
    return EXIT_SUCCESS;
}

int test_string_literal() {
    printf("Running %s\n", __FUNCTION__);
    return EXIT_SUCCESS;
}

int test_parse_tree() {
    printf("Running %s\n", __FUNCTION__);
    int val = yyparse();
    ProgramSource * output = x::top;
    Ident * Point = new Ident("Alec");
    VarDecl * x = new VarDecl(new TypeIdent("x"), new Ident("x"));
    VarDecl * y = new VarDecl(new TypeIdent("y"), new Ident("y"));
    std::vector<VarDecl *> decls) = {
        x, y
    };
    VarDeclList * varss = new VarDeclList(decls);
    StructDecl * strukt = new StructDecl(Point, varss);
    for (ASTNode* child: output->children) {
        StructDecl * struct = static_cast<StructDecl *>(child);
        assert(struct->name->id == "Point");
        assert(strct->members->decls[0]->var_name->id == "x");
    }
    return EXIT_SUCCESS;
}