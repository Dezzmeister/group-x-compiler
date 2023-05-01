#ifndef TESTS_UTILS_H
#define TESTS_UTILS_H

#define TEST_SUCCESS 0
#define TEST_FAILURE 1

#include <stdio.h>
#include <unordered_map>

#define expect(cond)  \
    if (!(cond)) {  \
        fprintf(stderr, "%s: failed at line %d: %s\n", __FILE__, __LINE__, #cond);    \
        return TEST_FAILURE;   \
    }

#define fail_test() \
    fprintf(stderr, "%s: failed at line %d\n", __FILE__, __LINE__); \
    return TEST_FAILURE;

typedef int (*TestFunc)(void);

namespace xtest {
    extern std::unordered_map<const char *, TestFunc> tests;
}

#endif