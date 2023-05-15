#include <stdlib.h>

#include "utils.h"
#include "../src/ast.h"
#include "../src/parseutils.h"
#include "../src/symtable.h"
#include "../src/errors.h"

void parser_tests() {
    xtest::tests["struct parse tree"] = []() {
        const char * code = R"(
            struct Point {
                int x.
                int y.
            }.
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

    xtest::tests["string literal declaration"] = []() {
        const char * code = R"(
            string a = "abc\n" + "".
            string b = "^$&Q*!)LL ".
        )";

        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;
        Location loc(0, 0, 0, 0);

        StringLiteral *stringA = new StringLiteral(loc, std::string("abc\n" + std::string("")).c_str());
        StringLiteral *stringB = new StringLiteral(loc, "^$&Q*!)LL ");
        StringLiteral *parsed_stringA = (StringLiteral *)output->find([](const ASTNode * node) {
            return node->get_kind() == StringLiteral::kind;
        });
        StringLiteral *parsed_stringB = (StringLiteral *)output->find([](const ASTNode * node) {
            return node->get_kind() == StringLiteral::kind;
        });

        expect(*stringA == *parsed_stringA);
        expect(*stringB == *parsed_stringB);

        delete stringA;
        delete stringB;

        return TEST_SUCCESS;
    };

    xtest::tests["New Struct"] = []() {
        const char * code = R"(
            struct dimension {
                float width.
                float height.
            };
            struct Shape {
                struct dimension area.
                string type.
            };

            int main(int argc) {
                mut struct Shape circle = new struct Shape.
                circle.type = "circle".
                circle.dimension.width = 5.
            }
        )";
        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;

        //expect
        return TEST_SUCCESS;
    };

    xtest::tests["Nested While"] = []() {
        const char * code = R"(
            int main() {
                mut int a.
                mut int b.
                mut int res.
                a = 1.
                b = 1.
                res = 0.
                while(a<15) {
                    while(b<15) {
                        res = res + b.
                        b = b+1.
                    }
                    a = a+1.
                }
            }
        )";

        /*ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;

        const std::string expected_parse = R"(
            int main() {
                    mut int a.
                    mut int b.
                    mut int res.
                    a = 1.
                    b = 1.
                    res = 0.
                    while (a < 15) {
                            while (b < 15) {
                                    res = res + b.
                                    b = b + 1.
                            }
                            a = a + 1.
                    }
            }
        )";
        expect(output == expected_parse);*/

        return TEST_SUCCESS;
    };

    xtest::tests["(Expected fail) No return, immutable variable"] = []() {
        const char * code = R"(
            int main() {
                int x = 0.
                x = 2.
            }
        )";

        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;
        SourceErrors &errors = report.sources[output];
        CompilerError err = errors[0];
        CompilerError err2 = errors[1];

        expect(errors.has_errors());
        expect(errors.error_count() == 1);
        expect(errors.type_errors.size() == 2);
        expect(err.level == Error);
        expect(err2.level == Error);
        expect(err.message == "Cannot assign to immutable");
        expect(err2.message = "No return");

        return TEST_SUCCESS;
    };

    xtest::tests["(Exepcted Fail) Missing curly brace"] = []() {
        const char * code = R"(
            int main() {
                mut int x = 0.
                x = x + 1.
                if (!x) {
                    return 1.
                return 0.
            }
        )";

        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;
        SourceErrors &errors = report.sources[output];
        CompilerError err = errors[0];

        expect(errors.has_errors());
        expect(errors.error_count() == 1);
        expect(errors.type_errors.size() == 1);
        expect(err.level == Error);
        expect(err.message == "Missing curly brace");
    };

    xtest::tests["(Expected Fail) Missing comma"] = []() {
        const char * code = R"(
            int difference(int lower int upper) {
                    return upper - lower.
            }

            int main() {
                int lower = 5.
                int upper = 10.
                int diff = difference.
                print(diff).
                return 0.
            }
        )";

        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;
        SourceErrors &errors = report.sources[output];
        CompilerError err = errors[0];

        expect(errors.has_errors());
        expect(errors.error_count() == 1);
        expect(errors.type_errors.size() == 1);
        expect(err.level == Error);
        expect(err.message == "Missing comma");
    };

    xtest::tests["(Expected Fail) Missing parenthesis"] = []() {
        const char * code = R"(
            int main() {
                print("Hello World").
                int x = 5 * (4 + 6.
                print(x).
                return 0.
            }
        )";

        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;
        SourceErrors &errors = report.sources[output];
        CompilerError err = errors[0];

        expect(errors.has_errors());
        expect(errors.error_count() == 1);
        expect(errors.type_errors.size() == 1);
        expect(err.level == Error);
        expect(err.message == "Missing parenthesis");
   `};

    xtest::tests["Array test"] = []() {
        const char * code = R"(
            int main() {
                mut int [5] arr = {0, 1, 2, 3, 4}.
                int x = arr[2].
                arr[2] = 3.
                return 0.
            }
        )";

        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;
        SourceErrors &errors = report.sources[output];
        SymbolTable * test_symtable = x::default_symtable();

        for (auto& out : output->children()) {
            if (out->get_kind() == VarDeclInit::kind) {
                VarDeclInit * vardecl = (VarDeclInit*) out;
                ArrayIndexExpr * rhs = (ArrayIndexExpr *) vardecl->init;
                const Expr * index = rhs->index;
                IntLiteral * index_as_int = (IntLiteral *) index;
                expect(index_as_int->value == 3);
                expect(rhs->type_equals(new TypeIdent(x::NULL_LOC, "float"));
            }
            if (out->get_kind() == Assignment::kind) {
                Assignment * assign = (Assignment*) out;
                ArrayIndexExpr * lhs = (ArrayIndexExpr *) assign->init;
                const Expr * index = lhs->index;
                IntLiteral * index_as_int = (IntLiteral *) index;
                expect(index_as_int->value == 2);
                expect(assign->type_check(test_symtable, &errors));
            }
        }
    };

    xtest::tests["Arrays test"] = []() {
        const char * code = R"(
            int main() {
                mut ints arr.
                arr[2] = 3.
            }
        )";
        /*ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;

        const std::string expected_parse = R"(
            int main() {
                    mut ints arr.
                    arr[2] = 3.
            }
        )";

        expect(huh == expected_parse);*/

        return TEST_SUCCESS;
    };

    xtest::tests["Continue test"] = []() {
        const char * code = R"(
            int main() {
                mut int count = 0.
                for (int i = 1. i < 6. i++) {
                    for int j = 1. j < 5. j++) {
                        if (j % 2 == 0) {
                            continue.
                        }
                        count++.
                    }
                    if (i % 3 == 0) {
                        continue.
                    }
                    count++.
                }
                return 0.
            }
        )";
        ParseResult result = x::parse_str(code);
        for (auto& out : output->children()) {
            if (out->get_kind() == VarDeclInit::kind) {
                VarDeclInit * vardecl = (VarDeclInit*) out;
                ArrayIndexExpr * rhs = (ArrayIndexExpr *) vardecl->init;
                const Expr * index = rhs->index;
                IntLiteral * index_as_int = (IntLiteral *) index;
                expect(index_as_int->value == 3);
            }
        }
        return TEST_SUCCESS;
    };
}
