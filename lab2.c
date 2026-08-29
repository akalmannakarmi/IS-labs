/* ============================================================
   Lab 2 : Playfair Cipher (Polygraphic Substitution)
   ------------------------------------------------------------
   Uses a 5x5 key square built from a keyword.
   I and J share one cell. Plaintext is broken into digraphs,
   an X is inserted between two identical letters and the text
   is padded with X to make its length even.
     - same row    : shift each letter one step right
     - same column : shift each letter one step down
     - otherwise   : swap columns (rectangle rule)
   ============================================================ */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SIZE 5

static char table[SIZE][SIZE];          /* the 5x5 key square   */
static int  pos[26][2];                 /* row,col of each letter */

/* Build the 5x5 square: keyword first, then remaining alphabet. */
static void build_table(const char *keyword)
{
    int used[26] = {0}, idx = 0, i;
    char c;

    /* put unique letters of the keyword first */
    for (i = 0; keyword[i]; i++) {
        c = toupper(keyword[i]);
        if (c == 'J') c = 'I';          /* I and J are combined */
        if (!isalpha(c) || used[c - 'A']) continue;
        used[c - 'A'] = 1;
        table[idx / SIZE][idx % SIZE] = c;
        pos[c - 'A'][0] = idx / SIZE;
        pos[c - 'A'][1] = idx % SIZE;
        if (c == 'I') {                 /* J points to I's cell  */
            pos['J' - 'A'][0] = idx / SIZE;
            pos['J' - 'A'][1] = idx % SIZE;
        }
        idx++;
    }
    /* fill the rest of the alphabet (skip J) */
    for (c = 'A'; c <= 'Z'; c++) {
        if (c == 'J' || used[c - 'A']) continue;
        table[idx / SIZE][idx % SIZE] = c;
        pos[c - 'A'][0] = idx / SIZE;
        pos[c - 'A'][1] = idx % SIZE;
        idx++;
    }
}

/* Encrypt (encode=1) or decrypt (encode=0) one digraph (a,b). */
static void process_pair(char a, char b, int encode, char *oa, char *ob)
{
    int step = encode ? 1 : SIZE - 1;   /* +1 row/col for encrypt, -1 for decrypt */
    int *pa = pos[a - 'A'];
    int *pb = pos[b - 'A'];

    if (pa[0] == pb[0]) {               /* same row: shift columns */
        *oa = table[pa[0]][(pa[1] + step) % SIZE];
        *ob = table[pb[0]][(pb[1] + step) % SIZE];
    } else if (pa[1] == pb[1]) {        /* same column: shift rows */
        *oa = table[(pa[0] + step) % SIZE][pa[1]];
        *ob = table[(pb[0] + step) % SIZE][pb[1]];
    } else {                            /* rectangle: swap columns */
        *oa = table[pa[0]][pb[1]];
        *ob = table[pb[0]][pa[1]];
    }
}

int main(void)
{
    char keyword[64], msg[256], clean[256], prep[512], out[512];
    char a, x, y;
    int i, n = 0, pn = 0, k = 0, choice, encode;

    printf("========== PLAYFAIR CIPHER ==========\n");
    printf("Enter keyword : ");
    fgets(keyword, sizeof keyword, stdin);
    keyword[strcspn(keyword, "\n")] = '\0';

    printf("Enter message : ");
    fgets(msg, sizeof msg, stdin);
    msg[strcspn(msg, "\n")] = '\0';

    printf("1. Encrypt  2. Decrypt : ");
    scanf("%d", &choice);
    encode = (choice == 1);             /* 1 = encrypt, else decrypt */

    build_table(keyword);

    /* keep only letters, uppercase, map J -> I */
    for (i = 0; msg[i]; i++)
        if (isalpha(msg[i])) {
            a = toupper(msg[i]);
            clean[n++] = (a == 'J') ? 'I' : a;
        }
    clean[n] = '\0';

    /* when encrypting: insert 'X' between repeated letters and pad
       to even length. During decryption the ciphertext already has
       its X-insertions in place, so it is processed as-is. */
    if (encode) {
        for (i = 0; i < n; i++) {
            prep[pn++] = clean[i];
            if (i + 1 < n && clean[i] == clean[i + 1])
                prep[pn++] = 'X';
        }
        if (pn % 2 == 1)
            prep[pn++] = 'X';
    } else {
        memcpy(prep, clean, (size_t)n);
        pn = n;
    }
    prep[pn] = '\0';

    /* process every digraph */
    for (i = 0; i < pn; i += 2) {
        process_pair(prep[i], prep[i + 1], encode, &x, &y);
        out[k++] = x;
        out[k++] = y;
    }
    out[k] = '\0';

    /* drop an 'X' that was only a padding filler after decryption */
    if (!encode && k > 0 && out[k - 1] == 'X')
        out[--k] = '\0';

    printf("Result        : %s\n", out);
    return 0;
}