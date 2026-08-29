/* ============================================================
   Lab 7 : Euclidean Algorithm (GCD + Extended)
   ------------------------------------------------------------
   1) Basic Euclidean algorithm  : gcd(a,b) = gcd(b, a mod b)
   2) Extended Euclidean alg.    : finds x,y such that
                                   a*x + b*y = gcd(a,b)
      - the modular inverse of a mod m is x when gcd(a,m)=1,
        and is used by RSA.
   ============================================================ */
#include <stdio.h>

/* simple Euclidean gcd */
static int gcd(int a, int b)
{
    int t;
    while (b != 0) {
        t = a % b;
        a = b;
        b = t;
    }
    return a;
}

/* extended: writes x,y with  a*x + b*y = gcd(a,b), returns gcd */
static int extended_gcd(int a, int b, int *x, int *y)
{
    int x1, y1, g;
    if (b == 0) {           /* base case: a*1 + 0*0 = a */
        *x = 1;
        *y = 0;
        return a;
    }
    g = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;                /* backtrack the coefficients */
    *y = x1 - (a / b) * y1;
    return g;
}

/* modular inverse of a mod m (m must be prime to a) */
static int mod_inverse(int a, int m)
{
    int x, y, g = extended_gcd(a, m, &x, &y);
    if (g != 1) return -1;              /* no inverse exists */
    x %= m;
    if (x < 0) x += m;
    return x;
}

int main(void)
{
    int a, b, x, y, g, choice;

    printf("========== EUCLIDEAN ALGORITHM ==========\n");
    printf("Enter a = ");
    scanf("%d", &a);
    printf("Enter b = ");
    scanf("%d", &b);

    printf("\n1. GCD only\n");
    printf("2. GCD + extended (a*x + b*y = gcd)\n");
    printf("3. Modular inverse of a mod b\n");
    printf("Choice : ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
        printf("gcd(%d, %d) = %d\n", a, b, gcd(a, b));
        break;
    case 2:
        g = extended_gcd(a, b, &x, &y);
        printf("gcd(%d, %d) = %d\n", a, b, g);
        printf("Coefficients : x = %d, y = %d\n", x, y);
        printf("Check : %d*%d + %d*%d = %d\n", a, x, b, y, a * x + b * y);
        break;
    case 3: {
        int inv = mod_inverse(a, b);
        if (inv < 0)
            printf("No inverse : gcd(%d,%d) != 1\n", a, b);
        else
            printf("Inverse of %d mod %d = %d\n", a, b, inv);
        break;
    }
    default:
        printf("Invalid choice\n");
    }
    return 0;
}