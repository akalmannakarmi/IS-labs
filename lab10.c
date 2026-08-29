/* ============================================================
   Lab 10 : Authentication System (Password + CAPTCHA)
   ------------------------------------------------------------
   Two-factor style login:
     1) Fixed password  (3 attempts allowed)
     2) CAPTCHA         (random letters/digits, 3 attempts)
   Access is granted only after BOTH checks pass. This models a
   real-world human-interaction check that stops bots.
   ============================================================ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    const char *PASSWORD = "tu@123";          /* stored password       */
    const char *pool   = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    char pass[64], capt[32], input[48];
    int tries, i, len;
    int pass_ok = 0;

    srand((unsigned)time(NULL));              /* seed for CAPTCHA rand */

    printf("========== AUTHENTICATION SYSTEM ==========\n");

    /* ---------- step 1 : password ---------- */
    printf("\n[Step 1] Enter password (3 attempts)\n");
    for (tries = 1; tries <= 3; tries++) {
        printf("Password : ");
        scanf("%63s", pass);
        if (strcmp(pass, PASSWORD) == 0) {
            pass_ok = 1;
            break;
        }
        printf("Wrong password! Attempts left : %d\n", 3 - tries);
    }

    if (!pass_ok) {
        printf("\nAccess DENIED (password failed).\n");
        return 1;
    }
    printf("[OK] Password accepted.\n");

    /* ---------- step 2 : CAPTCHA ---------- */
    printf("\n[Step 2] Solve the CAPTCHA (3 attempts)\n");
    for (tries = 1; tries <= 3; tries++) {
        len = 4 + rand() % 3;                 /* random length 4..6   */
        for (i = 0; i < len; i++)
            capt[i] = pool[rand() % (int)strlen(pool)];
        capt[len] = '\0';

        printf("CAPTCHA   : %s\n", capt);
        printf("Type it   : ");
        scanf("%47s", input);

        if (strcmp(capt, input) == 0) {
            printf("\n====== AUTHENTICATED! Access GRANTED ======\n");
            return 0;
        }
        printf("Wrong CAPTCHA! Attempts left : %d\n", 3 - tries);
    }

    printf("\nAccess DENIED (CAPTCHA failed).\n");
    return 1;
}