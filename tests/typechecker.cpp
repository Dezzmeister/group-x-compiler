#include "utils.h"
#include "../src/ast.h"
#include "../src/errors.h"
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
        Location loc(0, 0, 0, 0);

        TypeIdent x(loc, "X");
        TypeIdent y(loc, "Y");
        TypeIdent z(loc, "Z");
        TypeIdent w(loc, "W");

        try {
            expect(x.type_equals(&y, symtable));
            expect(y.type_equals(&x, symtable));

            expect(y.type_equals(&z, symtable));
            expect(z.type_equals(&y, symtable));

            expect(z.type_equals(&x, symtable));
            expect(x.type_equals(&z, symtable));

            expect(!w.type_equals(&y, symtable));
            expect(!y.type_equals(&w, symtable));
        } catch (CompilerError error) {
            error.print(stderr);
            fail_test();
        }

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
        Location loc(0, 0, 0, 0);

        TypeIdent arr1(loc, "Arr1");
        TypeIdent arr2(loc, "Arr2");
        TypeIdent arr3(loc, "Arr3");
        TypeIdent func1(loc, "Func1");
        TypeIdent func2(loc, "Func2");
        TypeIdent func3(loc, "Func3");

        try {
            expect(arr1.type_equals(&arr3, symtable));
            expect(!arr1.type_equals(&arr2, symtable));
            expect(func1.type_equals(&func3, symtable));
            expect(!func1.type_equals(&func2, symtable));
        } catch (CompilerError error) {
            error.print(stderr);
            fail_test();
        }

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
        Location loc(0, 0, 0, 0);

        TypeIdent strukt1(loc, "Strukt1");
        TypeIdent strukt2(loc, "Strukt2");
        TypeIdent strukt3(loc, "Strukt3");
        TypeIdent strukt4(loc, "Strukt4");

        try {
            expect(strukt1.type_equals(&strukt3, symtable));
            expect(!strukt1.type_equals(&strukt2, symtable));
            expect(!strukt1.type_equals(&strukt4, symtable));
        } catch (CompilerError error) {
            error.print(stderr);
            fail_test();
        }

        return TEST_SUCCESS;
    };

    xtest::tests["qualified type equality"] = []() {
        const char * code = R"(
            type X = (mut int)*;
            type Y = mut int*;
        )";

        ParseResult result = x::parse_str(code);
        SymbolTable * symtable = result.parser_state->symtable;
        Location loc(0, 0, 0, 0);

        PtrTypename * x = new PtrTypename(loc, new MutTypename(loc, new TypeIdent(loc, "int")));
        MutTypename * y = new MutTypename(loc, new PtrTypename(loc, new TypeIdent(loc, "int")));

        TypeIdent x_alias(loc, "X");
        TypeIdent y_alias(loc, "Y");

        try {
            expect(x->type_equals(&x_alias, symtable));
            expect(!x->type_equals(&y_alias, symtable));
            expect(!x->type_equals(y, symtable));

            expect(y->type_equals(&y_alias, symtable));
            expect(!y->type_equals(&x_alias, symtable));
        } catch (CompilerError error) {
            error.print(stderr);
            fail_test();
        }

        delete x;
        delete y;

        return TEST_SUCCESS;
    };
}
