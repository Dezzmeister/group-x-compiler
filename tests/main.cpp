#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "tests.h"
#include "utils.h"

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
    setup_tests();
    run_tests();

    return EXIT_SUCCESS;
}
