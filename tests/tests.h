#ifndef TESTS_TESTS_H
#define TESTS_TESTS_H

#define NUM_THREADS 2 

void parser_tests();
void typechecker_tests();

void setup_tests() {
    parser_tests();
    typechecker_tests();
}

#endif
