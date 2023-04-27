type Static1DArray = int[5];
type Static1DArray2 = (mut Static1DArray)*[10];
type Static2DArray = int[5][5];
type Static2DArray2 = (int[5])[5];
type FuncPtrArray = ([] -> void)[1];

void array_func((mut (int[5]))* five_guys) {
    *five_guys = {1, 2, 3, 4, 5};
};