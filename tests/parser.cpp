#include <stdlib.h>

#include "utils.h"
#include "../src/ast.h"
#include "../src/parseutils.h"
#include "../src/symtable.h"
#include "../src/error.h"

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
        // SymbolTable * symtable = result.parser_state->symtable;
        // ErrorReport &report = result.parser_state->errors;
        // SourceErrors &errors = report.sources[result.parser_state->top];

        // This symbol table is not accurate; we are only testing syntax here though
        // SymbolTable * test_symtable = x::default_symtable();
        Location loc(0, 0, 0, 0);
        // VarDecl * a = new VarDecl(loc, new TypeIdent(loc, "string"), new Ident(loc, "a"));
        // VarDecl * b = new VarDecl(loc, new TypeIdent(loc, "string"), new Ident(loc, "a"));

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
        //need to:
        // - add stringA and stringB to test_symtable
        // - check symbol tables
        // - check VarDeclList

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

        //Inaccurate symbol table
        SymbolTable * test_symtable = x::default_symtable();
        Location loc(0, 0, 0, 0);
        // Ident * dim = new Ident(loc, "dimension");
        VarDecl * width = new VarDecl(loc, new TypeIdent(loc, "float"), new Ident(loc, "width"));
        VarDecl * height = new VarDecl(loc, new TypeIdent(loc, "float"), new Ident(loc, "height"));
        std::vector<VarDecl *> dimDecls = {
            width, height
        };
        // VarDeclList * varsDim = new VarDeclList(loc, dimDecls);
        // StructTypename * dimStruct = new StructTypename(loc, varsDim, test_symtable);

        Ident * shape = new Ident(loc, "Shape");
        VarDecl * area = new VarDecl(loc, new TypeIdent(loc, "dimStruct"), new Ident(loc, "area"));
        VarDecl * type = new VarDecl(loc, new TypeIdent(loc, "string"), new Ident(loc, "type"));
        std::vector<VarDecl *> shapeDecls = {
            area, type
        };
        VarDeclList * varsShape = new VarDeclList(loc, shapeDecls);
        StructTypename * shapeStruct = new StructTypename(loc, varsShape, test_symtable);

        // Ident * circle = new Ident(loc, "circle");
        StructDecl * strukt = new StructDecl(loc, shape, shapeStruct);

        StructDecl * parsed_strukt = (StructDecl *) output->find([](const ASTNode * node) {
            return node->get_kind() == StructDecl::kind;
        });

        expect(*strukt == *parsed_strukt);

        //To-Do:
        // - assign circle.type = "circle"; AND circle.dimension.width = 5.;
        // - check circle.type and circle.dimension.width
        delete strukt;

        return TEST_SUCCESS;
    };

    xtests::tests["Nested While"] = []() {
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

        ParseResult result = x::parse_str(code);
        Program Source * output = result.parser_state->top;

        const char * expected_parse = R"(
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

        expect(output == expected_parse);

        return TEST_SUCCESS;
    };

    xtest::test["error test1"] = []() {
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
        expect(errors.type_errors.size() == 1);
        expect(err.level == Error);
        expect(err2.level == Error);
        expect(err.message == "Cannot assign to immutable");
        expect(err2.message = "No return");

        return TEST_SUCCESS;
    };

    xtest::test["arrays"] = []() {
        const char * code = R"(
            int main() {
                ints x;
            }
        )";

        ParseResult result = x::parse_str(code);
        ProgramSource * output = result.parser_state->top;

        const char * expected_parse = R"(
            int main() {
                    ints x;
            }
        )";

        expect(output == expected_parse)

        return TEST_SUCCESS;
    };
}
