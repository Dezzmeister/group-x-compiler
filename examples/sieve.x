// there is no way to dynamically allocate arrays currently.
// for an expression like int * primes = calloc(num) below
// the three address code currently has no of getting a pointer
// to memory on the heap.
int * primes = calloc(1).

mut int num_primes = 0.

void populate_primes(int upper_bound) {

    int * sieve = calloc(1).
    bool * is_prime = calloc(1).

    mut int i = 2.

    // statements are not allowed to be empty in our grammar
    // so we can't have the initialization part of the for loop
    // be an empty statement, hence the notused variable.

    for (int notused = 0; i * i < upper_bound; ++i) {
        if (is_prime[i]) {
            for (mut int j = i * i; j < upper_bound; ++j) {
                is_prime[j] = false.
            }.
            primes[num_primes] = i.
            ++num_primes.
        }.
    }.

    for (int not_used = 0; i < upper_bound; ++i) {
        if (is_prime[i]) {
            primes[num_primes] = i.
            ++num_primes.
        }.
    }.
}.

void main() {
    // 9 should be replaced with some input function.
    // Also we should find some way to get argv and argc
    int upper_bound = 9.
    populate_primes(upper_bound).
    for (mut int i = 0; i < num_primes; ++i) {
        print(primes[i]).
    }.
    print('\n').
}.
