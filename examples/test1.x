type X = [int, int] -> float;
int bruh = 15;

struct CoolStruct {
    int a;
    float b;
    X epic_func;
};

int cool_func_1(int a, int b) {
    mut int sum = a + b;
    (mut int)* epic_ptr = &sum;
    CoolStruct* cool_struct_ptr = 0; // comment!

    return epic_ptr;
};