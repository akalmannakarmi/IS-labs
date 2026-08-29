/* ============================================================
   Lab 8 : RSA (Rivest-Shamir-Adleman) Public-Key Cryptosystem
   ------------------------------------------------------------
   Key generation:
     p, q   : two primes                n = p*q
     phi(n) = (p-1)*(q-1)
     e      : 1 < e < phi, gcd(e,phi)=1 (public exponent)
     d      : e*d = 1 mod phi(n)        (private exponent)
   Encryption   : C = M^e mod n
   Decryption   : M = C^d mod n
   Simple version: every character of the message is encrypted
   as one number block (works because n is large enough).
   ============================================================ */
#include <stdio.h>
#include <string.h>

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

/* extended Euclid for the modular inverse needed for d */
static long long extended_gcd(long long a, long long b,
                              long long *x, long long *y)
{
    long long x1, y1, g;
    if (b == 0) { *x = 1; *y = 0; return a; }
    g = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

static long long mod_inverse(long long a, long long m)
{
    long long x, y, g = extended_gcd(a, m, &x, &y);
    if (g != 1) return -1;
    x %= m;
    if (x < 0) x += m;
    return x;
}

int main(void)
{
    unsigned long long p, q, n, phi, e;
    long long d;                        /* may be -1 if no inverse exists */
    char msg[256];
    unsigned long long blocks[256];
    int i, len;

    printf("========== RSA CRYPTOSYSTEM ==========\n");

    printf("Enter prime p         : ");
    scanf("%llu", &p);
    printf("Enter prime q         : ");
    scanf("%llu", &q);
    printf("Enter exponent e      : ");
    scanf("%llu", &e);

    n   = p * q;
    phi = (p - 1) * (q - 1);

    d = mod_inverse((long long)e, (long long)phi);
    if (d < 0) {
        printf("No inverse for e - pick another exponent.\n");
        return 1;
    }

    printf("Public key  (e, n) = (%llu, %llu)\n", e, n);
    printf("Private key (d, n) = (%llu, %llu)\n", d, n);

    printf("Enter message       : ");
    scanf(" %255[^\n]", msg);

    len = (int)strlen(msg);

    /* -------- encryption : C = M^e mod n -------- */
    for (i = 0; i < len; i++) {
        unsigned long long m = (unsigned char)msg[i];
        blocks[i] = mod_pow(m, e, n);
    }
    printf("\nEncrypted blocks (C = M^e mod n):\n");
    for (i = 0; i < len; i++)
        printf("%llu ", blocks[i]);
    printf("\n");

    /* -------- decryption : M = C^d mod n -------- */
    printf("\nDecrypted text (M = C^d mod n)  : ");
    for (i = 0; i < len; i++)
        putchar((int)mod_pow(blocks[i], d, n));
    printf("\n");

    return 0;
}