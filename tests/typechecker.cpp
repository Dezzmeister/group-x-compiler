#include "utils.h"
#include "../src/ast.h"
#include "../src/parseutils.h"
#include "../src/symtable.h"

void typechecker_tests() {
    xtest::tests["simple type alias equality"] = []() {
        const char * code = R"(
            type X = int;
            type Y = X;
            type Z = int;

            type W = float;
        )";

        ParseResult result = x::parse_str(code);
        SymbolTable * symtable = result.parser_state->symtable;

        TypeIdent x("X");
        TypeIdent y("Y");
        TypeIdent z("Z");
        TypeIdent w("W");

        expect(x.type_equals(&y, symtable));
        expect(y.type_equals(&x, symtable));

        expect(y.type_equals(&z, symtable));
        expect(z.type_equals(&y, symtable));

        expect(z.type_equals(&x, symtable));
        expect(x.type_equals(&z, symtable));

        expect(!w.type_equals(&y, symtable));
        expect(!y.type_equals(&w, symtable));

        return TEST_SUCCESS;
    };

    xtest::tests["complex type alias equality"] = []() {
        const char * code = R"(
            // Arr1 == Arr3
            type Arr1 = int[5];
            type Arr2 = float[5];
            type Arr3 = int[5];

            // Func1 == Func3
            type Func1 = [int, int, float] -> float;
            type Func2 = [int, float, int] -> float;
            type Func3 = [int, int, float] -> float;
        )";

        ParseResult result = x::parse_str(code);
        SymbolTable * symtable = result.parser_state->symtable;

        TypeIdent arr1("Arr1");
        TypeIdent arr2("Arr2");
        TypeIdent arr3("Arr3");
        TypeIdent func1("Func1");
        TypeIdent func2("Func2");
        TypeIdent func3("Func3");

        expect(arr1.type_equals(&arr3, symtable));
        expect(!arr1.type_equals(&arr2, symtable));
        expect(func1.type_equals(&func3, symtable));
        expect(!func1.type_equals(&func2, symtable));

        return TEST_SUCCESS;
    };

    xtest::tests["struct type equality"] = []() {
        const char * code = R"(
            type AliasedInt = int;
            
            // Strukt1 == Strukt3
            struct Strukt1 {
                int x;
                int y;
            };

            struct Strukt2 {
                int x;
                float y;
            };

            struct Strukt3 {
                AliasedInt x;
                int y;
            };

            struct Strukt4 {
                int x;
                int z;
            };
        )";

        ParseResult result = x::parse_str(code);
        SymbolTable * symtable = result.parser_state->symtable;

        TypeIdent strukt1("Strukt1");
        TypeIdent strukt2("Strukt2");
        TypeIdent strukt3("Strukt3");
        TypeIdent strukt4("Strukt4");

        expect(strukt1.type_equals(&strukt3, symtable));
        expect(!strukt1.type_equals(&strukt2, symtable));
        expect(!strukt1.type_equals(&strukt4, symtable));

        return TEST_SUCCESS;
    };
}
