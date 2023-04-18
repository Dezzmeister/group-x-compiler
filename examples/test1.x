// This is a function type. It uses square brackets instead of parentheses because of
// parser conflicts but with the symbol table we can probably change this back to parens.
type X = [int, int] -> float;

// Tuple type with square brackets for the same reason
type Tuple1 = [int, float, X];

int bruh = 15;

struct CoolStruct {
    int a;
    float b;
    X epic_func;
};

int fac(int n) {
    if (n) {
        return n * fac(n - 1);
    };

    return 1;
};

int cool_func_1(int a, int b) {
    mut int sum = a + b;
    (mut int)* epic_ptr = &sum;
    CoolStruct* cool_struct_ptr = 0;

    if (true) {
        int z = fac(10);
    }; // TODO: Remove trailing comma from grammar for if/while/for/etc.

    return epic_ptr;
};