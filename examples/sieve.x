primes = List(upper_bound * sizeof(int));

void populate_primes(int upper_bound) {

    // The syntax for a dynamically allocated array 
    // is takan from Section 9.1 of the lrm, but it can change if needed.
    sieve = List(upper_bound * sizeof(int));    
    is_prime = List(upper_bound * sizeof(boolean));
    

    int i = 2;

    // statements are not allowed to be empty in our grammar, hence not_used.
    for (int notused = 0; i * i < upper_bound; ++i) {
        if (is_prime[i]) {
            for (int j = i * i; j < upper_bound; j = j + i) {
                is_prime[j] = false; 
            }
            primes[num_primes++] = i;
        }
    }
    for (int not_used = 0; i < upper_bound; ++i) {
        if (is_prime[i]) {
            primes[num_primes++] = i;
        }
    }
}

void main() {
    // 9 should be replaced with some input function. 
    // Also we should find some way to get argv and argc
    int upper_bound = 9;
    populate_primes(upper_bound)
    for (int i = 0; i < num_primes; ++i) {
        print(primes[i])
    }
    print('\n');
}