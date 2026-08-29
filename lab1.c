/* ============================================================
   Lab 1 : Caesar Cipher (Substitution Cipher)
   ------------------------------------------------------------
   Each letter is shifted by a fixed key value K.
     Encryption : C = (P + K) mod 26
     Decryption : P = (C - K) mod 26
   Works on Letters only; digits and symbols stay unchanged.
   ============================================================ */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    char text[256], result[256];
    int key, choice, i;

    printf("========== CAESAR CIPHER ==========\n");

    printf("Enter text       : ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = '\0';        /* remove newline */

    printf("Enter key (1-25) : ");
    scanf("%d", &key);

    printf("1. Encrypt  2. Decrypt : ");
    scanf("%d", &choice);

    key = key % 26;
    if (choice == 2)         /* decrypt means shift in reverse */
        key = (26 - key) % 26;

    /* apply the shift to every letter */
    for (i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (isupper(c))
            result[i] = (char)(((c - 'A' + key) % 26) + 'A');
        else if (islower(c))
            result[i] = (char)(((c - 'a' + key) % 26) + 'a');
        else
            result[i] = c;          /* leave spaces, digits, etc. */
    }
    result[i] = '\0';

    printf("Result           : %s\n", result);
    return 0;
}