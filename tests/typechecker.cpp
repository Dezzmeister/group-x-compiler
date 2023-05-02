#include <memory>

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

    xtest::tests["simple expr typeof"] = []() {
        const char * code = R"(
            type X = int;
            type Z = float;

            int p = 5;
            X q = 9;

            X r = p + q; @1
            float w = p * 5.0; @2

            bool x = r < w || true; @3

            int[5] y = {1, 2, 3 + 4, r, 6 % 7}; @4
        )";

        ParseResult result = x::parse_str(code);
        SymbolTable * symtable = result.parser_state->symtable;
        VarDeclInit * r_decl = (VarDeclInit *) result.parser_state->debug_stmts[1];
        VarDeclInit * w_decl = (VarDeclInit *) result.parser_state->debug_stmts[2];
        VarDeclInit * x_decl = (VarDeclInit *) result.parser_state->debug_stmts[3];
        VarDeclInit * y_decl = (VarDeclInit *) result.parser_state->debug_stmts[4];

        TypeIdent int_type(x::NULL_LOC, "int");
        TypeIdent float_type(x::NULL_LOC, "float");
        TypeIdent bool_type(x::NULL_LOC, "bool");

        try {
            std::unique_ptr<Typename> r_type(r_decl->init->type_of(symtable));
            std::unique_ptr<Typename> w_type(w_decl->init->type_of(symtable));

            expect(r_type->type_equals(&int_type, symtable));
            expect(w_type->type_equals(&float_type, symtable));
            expect(int_type.type_equals(r_type.get(), symtable));
            expect(float_type.type_equals(w_type.get(), symtable));

            expect(!r_type->type_equals(&float_type, symtable));
            expect(!w_type->type_equals(&int_type, symtable));
        } catch (CompilerError error) {
            error.print(stderr);
            fail_test();
        }

        try {
            std::unique_ptr<Typename> x_type(x_decl->init->type_of(symtable));

            expect(x_type->type_equals(&bool_type, symtable));
            expect(bool_type.type_equals(x_type.get(), symtable));

            expect(!x_type->type_equals(&int_type, symtable));
            expect(!x_type->type_equals(&float_type, symtable));
        } catch (CompilerError error) {
            error.print(stderr);
            fail_test();
        }

        try {
            std::unique_ptr<Typename> y_type(y_decl->init->type_of(symtable));
            std::unique_ptr<StaticArrayTypename> y_decl_type(new StaticArrayTypename(x::NULL_LOC, new TypeIdent(x::NULL_LOC, "int"),
                    new IntLiteral(x::NULL_LOC, 5)));

            expect(y_type->type_equals(y_decl_type.get(), symtable));
            expect(!y_type->type_equals(&int_type, symtable));
        } catch (CompilerError error) {
            error.print(stderr);
            fail_test();
        }

        return TEST_SUCCESS;
    };

    xtest::tests["casting rules followed"] = []() {
        const char * code = R"(
            int[1] arr1 = {4};
            [int] tup1 = [7];
            int[3] arr2 = {1, 2, 3};
            mut int int1 = 5;

            float p = 5 as float;               @1
            int q = arr1 as int;                @2
            int r = tup1 as int;                @3
            float[3] s = arr2 as float[3];      @4
            mut float t = int1 as mut float;    @5
            int u = &s as int;                  @6
            (mut int)* v = 0 as (mut int)*;     @7
        )";

        ParseResult result = x::parse_str(code);
        SymbolTable * symtable = result.parser_state->symtable;
        VarDeclInit * p_decl = (VarDeclInit *) result.parser_state->debug_stmts[1];
        VarDeclInit * q_decl = (VarDeclInit *) result.parser_state->debug_stmts[2];
        VarDeclInit * r_decl = (VarDeclInit *) result.parser_state->debug_stmts[3];
        VarDeclInit * s_decl = (VarDeclInit *) result.parser_state->debug_stmts[4];
        VarDeclInit * t_decl = (VarDeclInit *) result.parser_state->debug_stmts[5];
        VarDeclInit * u_decl = (VarDeclInit *) result.parser_state->debug_stmts[6];
        VarDeclInit * v_decl = (VarDeclInit *) result.parser_state->debug_stmts[7];

        try {
            std::unique_ptr<Typename> p_type(p_decl->init->type_of(symtable));
            std::unique_ptr<Typename> q_type(q_decl->init->type_of(symtable));
            std::unique_ptr<Typename> r_type(r_decl->init->type_of(symtable));
            std::unique_ptr<Typename> s_type(s_decl->init->type_of(symtable));
            std::unique_ptr<Typename> t_type(t_decl->init->type_of(symtable));
            std::unique_ptr<Typename> u_type(u_decl->init->type_of(symtable));
            std::unique_ptr<Typename> v_type(v_decl->init->type_of(symtable));

            expect(p_type->type_equals(p_decl->decl->type_name, symtable));
            expect(q_type->type_equals(q_decl->decl->type_name, symtable));
            expect(r_type->type_equals(r_decl->decl->type_name, symtable));
            expect(s_type->type_equals(s_decl->decl->type_name, symtable));
            expect(t_type->type_equals(t_decl->decl->type_name, symtable));
            expect(u_type->type_equals(u_decl->decl->type_name, symtable));
            expect(v_type->type_equals(v_decl->decl->type_name, symtable));
        } catch (CompilerError error) {
            error.print(stderr);
            fail_test();
        }

        return TEST_SUCCESS;
    };

    xtest::tests["casting rules broken"] = []() {
        const char * code = R"(
            float f1 = 4.0;
            [int, int] tup1 = [1, 2];
            
            int func1(int x) {
                return 4;
            };

            int p = f1 as int;                              @1
            [float, float] q = tup1 as [float, float];      @2
            int r = tup1 as int;                            @3
            [int] -> float s = func1 as [int] -> float;     @4
        )";

        ParseResult result = x::parse_str(code);
        SymbolTable * symtable = result.parser_state->symtable;
        VarDeclInit * p_decl = (VarDeclInit *) result.parser_state->debug_stmts[1];
        VarDeclInit * q_decl = (VarDeclInit *) result.parser_state->debug_stmts[2];
        VarDeclInit * r_decl = (VarDeclInit *) result.parser_state->debug_stmts[3];
        VarDeclInit * s_decl = (VarDeclInit *) result.parser_state->debug_stmts[4];

        try {
            std::unique_ptr<Typename> p_type(p_decl->init->type_of(symtable));

            fail_test();
        } catch (CompilerError error) {
            
        }

        try {
            std::unique_ptr<Typename> q_type(q_decl->init->type_of(symtable));

            fail_test();
        } catch (CompilerError error) {

        }

        try {
            std::unique_ptr<Typename> r_type(r_decl->init->type_of(symtable));

            fail_test();
        } catch (CompilerError error) {

        }

        try {
            std::unique_ptr<Typename> s_type(s_decl->init->type_of(symtable));

            fail_test()
        } catch (CompilerError error) {

        }

        return TEST_SUCCESS;
    };
}
