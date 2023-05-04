// This is a function type. It uses square brackets instead of parentheses because of
// parser conflicts but with the symbol table we can probably change this back to parens.
type X = [int, int] -> float;

// Tuple type with square brackets for the same reason
type Tuple1 = [int, float, X];

[int] -> void int_eater;

int bruh = 15;

struct CoolStruct {
    int a;
    float b;
    X epic_func;
    struct {
        int x;
        int y;
    } pt1;
    struct {
        int u;
        int v;
        [int, int] -> void int_eater_2;
        struct {
            float z;
        } strukt2;
    } strukt1;
};

type EpicStruct = struct {
    int x;
    int y;
};

int fac(int n) {
    if (n > 0) {
        return n * fac(n - 1);
    };

    return 1;
};

int cool_func_1(int a, int b) {
    mut int sum = a + b;
    (mut int)* epic_ptr = &sum;
    CoolStruct* cool_struct_ptr = 0 as CoolStruct*;

    if (true) {
        int z = fac(10);
    }; // TODO: Remove trailing semicolon from grammar for if/while/for/etc.

    int_eater(4);

    for (mut int i = 0; i <= 100; i = i + 1) {
        print(i_to_str(i));
    };

    bool b0 = true && false;
    bool b1 = b0 || false;
    bool b2 = (b0 and b1) or true;
    bool b3 = 5 in {1, 2, 3, 4, 5};
    bool b4 = fac(3) not in {1, 2, 3, 4, 5};
    bool b5 = not (fac(3) == 1);
    bool b6 = !b5;
    bool b7 = !!!true;

    return epic_ptr as int;
};

void int_eater(int x) {
    print(i_to_str(x));
};
