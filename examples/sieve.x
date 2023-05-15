int * primes.
mut int num_primes = 0.

void populate_primes(int upper_bound) {

    int * sieve = calloc(upper_bound).
    bool * is_prime = calloc(upper_bound).

    mut int i = 2.

    for (int notused = 0; i * i < upper_bound; i = i + 1) {
        if (is_prime[i]) {
            for (mut int j = i * i; j < upper_bound; j = j + 1) {
                is_prime[j] = false.
            }.
            primes[num_primes] = i.
            num_primes = num_primes + 1.
        }.
    }.

    for (int not_used = 0; i < upper_bound; i = i + 1) {
        if (is_prime[i]) {
            primes[num_primes] = i.
            num_primes = num_primes + 1.
        }.
    }.
}.

void main(int argc, char * argv[]) {
    int upper_bound = i_to_str(argv[0]).
    populate_primes(upper_bound).
    for (mut int i = 0; i < num_primes; i = i + 1) {
        print(primes[i]).
    }.
    print('\n').
}.
