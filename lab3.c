/* ============================================================
   Lab 3 : Rail Fence Cipher (Transposition Cipher)
   ------------------------------------------------------------
   The plaintext is written in a zig-zag pattern across
   KEY rails, then read off row by row to get the cipher.
   Decryption re-places every cipher letter into the same
   zig-zag pattern and reads it back normally.
   ============================================================ */
#include <stdio.h>
#include <string.h>

/* Returns the rail number (0..depth-1) for position i. */
static int rail_of(int i, int depth)
{
    int period = 2 * (depth - 1);       /* length of one zig-zag cycle */
    int r = i % period;
    return (r < depth) ? r : period - r;
}

int main(void)
{
    char msg[256], cipher[256], plain[256];
    int depth, i, n, len = 0;

    printf("========== RAIL FENCE CIPHER ==========\n");
    printf("Enter text     : ");
    fgets(msg, sizeof msg, stdin);
    msg[strcspn(msg, "\n")] = '\0';

    printf("Enter rails(2+) : ");
    scanf("%d", &depth);
    n = (int)strlen(msg);

    cipher[len] = '\0';

    /* ---------- ENCRYPTION: build each rail then concatenate ---------- */
    {
        char rails[8][256];
        int counts[8] = {0};

        if (depth < 2) { printf("Depth must be >= 2\n"); return 1; }
        if (depth > 8)  { printf("Depth must be <= 8\n"); return 1; }

        /* assign every character to its rail */
        for (i = 0; i < n; i++) {
            int r = rail_of(i, depth);
            rails[r][counts[r]++] = msg[i];
        }
        /* read the rails top to bottom */
        {
            int pos = 0;
            for (i = 0; i < depth; i++)
                for (len = 0; len < counts[i]; len++)
                    cipher[pos++] = rails[i][len];
        }
        cipher[n] = '\0';
    }

    /* ---------- DECRYPTION: rebuild the pattern ---------- */
    {
        int start[8], counts[8] = {0}, offset[8];

        /* count how many cipher letters fall on each rail */
        for (i = 0; i < n; i++)
            counts[rail_of(i, depth)]++;

        /* starting offset of every rail inside the ciphertext */
        start[0] = 0;
        for (i = 1; i < depth; i++)
            start[i] = start[i - 1] + counts[i - 1];

        /* copy offsets separately as we walk through cipher */
        for (i = 0; i < depth; i++)
            offset[i] = start[i];

        /* refill each zig-zag position in order */
        for (i = 0; i < n; i++) {
            int r = rail_of(i, depth);
            plain[i] = cipher[offset[r]++];
        }
        plain[n] = '\0';
    }

    printf("Cipher text   : %s\n", cipher);
    printf("Decrypted text: %s\n", plain);

    if (strcmp(plain, msg) == 0)
        printf("== Round-trip OK (decryption matches original) ==\n");
    else
        printf("== Warning: decryption does not match original ==\n");

    return 0;
}