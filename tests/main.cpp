#include <stdio.h>
#include <stdlib.h>
#include <vector>

typedef int (*test_func)(void);

extern int test_parse_tree();

// Tests to be run are declared 'extern' and stored in this vector
std::vector<test_func> TESTS = {
    test_parse_tree
};

int main() {
    // Loop through the test vector and run each test. 'auto' is a C++ keyword
    // that provides some type inference, and C++ has foreach loops
    for (const auto test : TESTS) {
        // A test is successful if it returns EXIT_SUCCESS
        const int retval = test();
        if (retval != EXIT_SUCCESS) {
            fprintf(stderr, "Test failed with code %d. Exiting...\n", retval);
            return EXIT_FAILURE;
        }
    }

    printf("All tests passed\n");
    return EXIT_SUCCESS;
}
