/**
 * ================================= Test Setup =================================
 * Test suites are in separate .cpp files. Each one exports a function whose
 * prototype is in tests.h. The functions insert key/value pairs into xtest::tests,
 * which is a map from strings to functions. The string key is the name of the test
 * case, and the function is a void -> int that runs the test case. To add a test
 * case, insert a new KV pair into xtest::tests with the name of the test case and the
 * test code. typechecker.cpp and parser.cpp have examples of this. To add a test suite,
 * create a file for it and a void -> void in which you add the test cases to the
 * map. Put a function declaration for the test suite function in tests.h and call it
 * in setup_tests().
 *
 * Each test case function returns an error code (TEST_SUCCESS or TEST_FAILURE). The
 * 'expect()' macro works like assert, except instead of terminating the program on failure
 * it prints the file, line no, and failing expression, and returns TEST_FAILURE from the
 * test case. 'fail_test()' just returns TEST_FAILURE unconditionally. The tests should
 * not terminate the program; the error codes are used to count the number of successful
 * tests and print it at the end. The tests are run by several pthreads working in parallel
 * which has a couple of advantages:
 *  - random test order to catch heisenbugs related to mutation of state
 *  - tests run faster
 * The number of threads to use is in tests.h (NUM_THREADS).
 *
 * In tests only, an '@' symbol followed by an integer can come after a statement + semicolon,
 * and it will be accepted by the grammar. The integer is an index, indexing the statement so
 * that it can be looked up in the test. See typechecker.cpp for an example of this.
 */
#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "tests.h"
#include "utils.h"
#include "../src/parseutils.h"

struct NamedTest {
    const char * name;
    TestFunc func;
};

struct ThreadArg {
    std::queue<NamedTest> queue;
    int successes;
};

std::unordered_map<const char *, TestFunc> xtest::tests = std::unordered_map<const char *, TestFunc>();

void * thread_main(void * thread_arg) {
    struct ThreadArg * arg = (struct ThreadArg *) thread_arg;
    int successes = 0;

    while (!arg->queue.empty()) {
        NamedTest test = arg->queue.front();
        arg->queue.pop();
        printf("Running \"%s\"...\n", test.name);
        fflush(stdout);
        int retval = test.func();

        if (retval == TEST_SUCCESS) {
            successes++;
        }
    }

    arg->successes = successes;

    return NULL;
}

void run_tests() {
    struct ThreadArg args[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    int i = 0;

    for (auto &item : xtest::tests) {
        NamedTest test = {
            .name = item.first,
            .func = item.second
        };
        args[i].queue.push(test);
        i = (i + 1) % NUM_THREADS;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        int error = pthread_create(threads + i, NULL, thread_main, args + i);

        if (error) {
            perror("Error: ");
            exit(1);
        }
    }

    int total_tests = xtest::tests.size();
    int successes = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        successes += args[i].successes;
    }

    printf("%d/%d tests passed\n", successes, total_tests);
}

int main() {
    x::setup_symtable();
    setup_tests();
    run_tests();

    return EXIT_SUCCESS;
}
