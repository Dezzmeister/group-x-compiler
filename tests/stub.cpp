#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
    struct Point {
        Ident * Point = new Ident("Alec");
        VarDecl * x = new VarDecl(Ident("x"), Typename);
        VarDecl * y = new VarDecl(Ident("y"), Typename);
        std::vector<VarDecl *> decls) = {
            x, y
        };
        new VarDeclList(std::vector<VarDecl *> decls);
        new StructDecl(point, decls);
    };
    new struct Point Alec {};
    assert(Alec);
    assert(Alec.x == NULL);
    assert(Alec.y == NULL);
    return EXIT_SUCCESS;
}