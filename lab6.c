/* ============================================================
   Lab 6 : Primality Testing with Miller-Rabin
   ------------------------------------------------------------
   Probabilistic test:
     * write n-1 = d * 2^s  (d odd)
     * pick base a, compute x = a^d mod n
     * if x == 1 or x == n-1 the base passes
     * otherwise square x up to s-1 times: pass if a value
       becomes n-1
   We test with the fixed bases {2,3,5,7,11,13} which makes
   the test deterministic for all numbers below 3.4e14.
   ============================================================ */
#include <stdio.h>

/* fast modular exponentiation: (base^exp) mod mod */
static unsigned long long mod_pow(unsigned long long base,
                                  unsigned long long exp,
                                  unsigned long long mod)
{
    unsigned long long r = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) r = (r * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return r;
}

/* check n for one base a. Returns 1 = probably prime. */
static int miller_rabin(unsigned long long n, unsigned long long a)
{
    unsigned long long d = n - 1;
    int s = 0, i;
    unsigned long long x;

    /* factor powers of two out of n-1: n-1 = d * 2^s */
    while ((d & 1ULL) == 0) { d >>= 1; s++; }

    x = mod_pow(a, d, n);
    if (x == 1 || x == n - 1)
        return 1;                       /* base says "prime" */

    for (i = 0; i < s - 1; i++) {
        x = (x * x) % n;
        if (x == n - 1)
            return 1;
    }
    return 0;                           /* base says "composite" */
}

/* deterministic for all n < 3.4e14 using the six bases below */
static int is_prime(unsigned long long n)
{
    const unsigned long long bases[] = {2, 3, 5, 7, 11, 13};
    size_t i;

    if (n < 2) return 0;
    if (n == 2 || n == 3) return 1;

    /* quick rejection of even numbers */
    if ((n & 1ULL) == 0) return 0;

    /* small special cases fall through cleanly anyway */
    for (i = 0; i < sizeof bases / sizeof bases[0]; i++) {
        if (bases[i] >= n) break;
        if (!miller_rabin(n, bases[i]))
            return 0;
    }
    return 1;
}

int main(void)
{
    unsigned long long n;

    printf("========== MILLER-RABIN PRIMALITY TEST ==========\n");
    printf("Enter a number : ");
    scanf("%llu", &n);

    if (is_prime(n))
        printf("%llu is PRIME (probably prime - deterministic for this range)\n", n);
    else
        printf("%llu is COMPOSITE (not prime)\n", n);
    return 0;
}