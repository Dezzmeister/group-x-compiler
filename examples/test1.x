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

    for (mut int i = 0; i <= 100; i = i + 1) {
        print(i);
    };

    bool b0 = true && false;
    bool b1 = b0 || false;
    bool b2 = (b0 and b1) or true;
    bool b3 = 5 in {1, 2, 3, 4, 5};
    bool b4 = fac(3) not in {1, 2, 3, 4, 5};
    bool b5 = not (fac(3) == 1);
    bool b6 = !b5;
    bool b7 = !!!true;

    return epic_ptr;
};