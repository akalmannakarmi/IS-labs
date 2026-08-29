/* ============================================================
   Lab 5 : AES - Advanced Encryption Standard (AES-128)
   ------------------------------------------------------------
   Only the S-box, inverse S-box and Rcon live in aes_tables.h;
   this file holds the core cipher logic:
     SubBytes, ShiftRows, MixColumns, AddRoundKey
     + key expansion (128-bit key, 10 rounds)
   State is stored column-major (state[col*4 + row]) exactly
   as in the FIPS-197 specification.
   Inputs/outputs are 16-byte blocks as 32 hex characters.
   ============================================================ */
#include <stdio.h>
#include <string.h>
#include "aes_tables.h"

#define Nk 4     /* words in the key (128 bits) */
#define Nr 10    /* number of rounds            */

/* ------- Galois-field helpers ----------------------------------- */
static unsigned char xtime(unsigned char b)   /* multiply by x = 2 */
{
    return (unsigned char)((b << 1) ^ ((b & 0x80) ? 0x1b : 0x00));
}

static unsigned char gmul(unsigned char a, unsigned char b) /* any (*) */
{
    unsigned char r = 0;
    while (a) {
        if (a & 1) r ^= b;
        a >>= 1;
        b = xtime(b);
    }
    return r;
}

/* ------- the four AES steps ------------------------------------- */
static void add_round_key(unsigned char st[16], const unsigned char rk[16])
{
    int i;
    for (i = 0; i < 16; i++) st[i] ^= rk[i];
}

static void sub_bytes(unsigned char st[16])
{
    int i;
    for (i = 0; i < 16; i++) st[i] = SBOX[st[i]];
}

static void inv_sub_bytes(unsigned char st[16])
{
    int i;
    for (i = 0; i < 16; i++) st[i] = INV_SBOX[st[i]];
}

static void shift_rows(unsigned char st[16])
{
    unsigned char t;
    /* row 1 : shift left by 1 */
    t = st[1];  st[1] = st[5];   st[5] = st[9];
    st[9] = st[13]; st[13] = t;
    /* row 2 : shift left by 2 (swap pairs) */
    t = st[2];  st[2] = st[10];  st[10] = t;
    t = st[6];  st[6] = st[14];  st[14] = t;
    /* row 3 : shift left by 3 */
    t = st[3];  st[3] = st[15];  st[15] = st[11];
    st[11] = st[7]; st[7] = t;
}

static void inv_shift_rows(unsigned char st[16])
{
    unsigned char t;
    /* row 1 : shift right by 1 */
    t = st[13]; st[13] = st[9];  st[9] = st[5];
    st[5] = st[1]; st[1] = t;
    /* row 2 : shift right by 2 */
    t = st[2];  st[2] = st[10];  st[10] = t;
    t = st[6];  st[6] = st[14];  st[14] = t;
    /* row 3 : shift right by 3 */
    t = st[7];  st[7] = st[11];  st[11] = st[15];
    st[15] = st[3]; st[3] = t;
}

static void mix_columns(unsigned char st[16])
{
    int c;
    for (c = 0; c < 4; c++) {
        unsigned char *s = st + 4 * c;         /* this column        */
        unsigned char a0 = s[0], a1 = s[1], a2 = s[2], a3 = s[3];
        unsigned char t = a0 ^ a1 ^ a2 ^ a3;

        s[0] = a0 ^ t ^ xtime(a0 ^ a1);
        s[1] = a1 ^ t ^ xtime(a1 ^ a2);
        s[2] = a2 ^ t ^ xtime(a2 ^ a3);
        s[3] = a3 ^ t ^ xtime(a3 ^ a0);
    }
}

static void inv_mix_columns(unsigned char st[16])
{
    int c;
    for (c = 0; c < 4; c++) {
        unsigned char *s = st + 4 * c;
        unsigned char a0 = s[0], a1 = s[1], a2 = s[2], a3 = s[3];

        s[0] = gmul(a0, 0x0e) ^ gmul(a1, 0x0b) ^ gmul(a2, 0x0d) ^ gmul(a3, 0x09);
        s[1] = gmul(a0, 0x09) ^ gmul(a1, 0x0e) ^ gmul(a2, 0x0b) ^ gmul(a3, 0x0d);
        s[2] = gmul(a0, 0x0d) ^ gmul(a1, 0x09) ^ gmul(a2, 0x0e) ^ gmul(a3, 0x0b);
        s[3] = gmul(a0, 0x0b) ^ gmul(a1, 0x0d) ^ gmul(a2, 0x09) ^ gmul(a3, 0x0e);
    }
}

/* ------- key schedule: 16 bytes -> 176 bytes -------------------- */
static void key_expansion(const unsigned char key[16], unsigned char rk[176])
{
    unsigned char t[4];
    int i, j;

    for (i = 0; i < 16; i++) rk[i] = key[i];          /* first word(s) */

    for (i = 4; i < 44; i++) {
        for (j = 0; j < 4; j++) t[j] = rk[(i - 1) * 4 + j];   /* previous w */

        if (i % 4 == 0) {                            /* every 4th word */
            unsigned char tmp = t[0];                /* RotWord        */
            t[0] = t[1]; t[1] = t[2]; t[2] = t[3]; t[3] = tmp;
            for (j = 0; j < 4; j++) t[j] = SBOX[t[j]]; /* SubWord       */
            t[0] ^= RCON[i / 4 - 1];                 /* Rcon           */
        }

        for (j = 0; j < 4; j++)
            rk[i * 4 + j] = rk[(i - 4) * 4 + j] ^ t[j];
    }
}

/* ------- single-block encryption / decryption -------------------- */
static void aes_encrypt(const unsigned char in[16], unsigned char out[16],
                        const unsigned char rk[176])
{
    unsigned char st[16];
    int round;

    memcpy(st, in, 16);

    add_round_key(st, rk);                           /* round 0 key */

    for (round = 1; round <= Nr - 1; round++) {      /* rounds 1..9  */
        sub_bytes(st);
        shift_rows(st);
        mix_columns(st);
        add_round_key(st, rk + round * 16);
    }

    sub_bytes(st);                                   /* final round  */
    shift_rows(st);
    add_round_key(st, rk + Nr * 16);

    memcpy(out, st, 16);
}

static void aes_decrypt(const unsigned char in[16], unsigned char out[16],
                        const unsigned char rk[176])
{
    unsigned char st[16];
    int round;

    memcpy(st, in, 16);

    add_round_key(st, rk + Nr * 16);                 /* last key first */

    for (round = Nr - 1; round >= 1; round--) {      /* rounds 9..1  */
        inv_shift_rows(st);
        inv_sub_bytes(st);
        add_round_key(st, rk + round * 16);
        inv_mix_columns(st);
    }

    inv_shift_rows(st);                              /* final round  */
    inv_sub_bytes(st);
    add_round_key(st, rk);

    memcpy(out, st, 16);
}

/* ------- hex helpers --------------------------------------------- */
static int hexval(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int hex2bytes(const char *s, unsigned char *out, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        int hi = hexval(s[2 * i]), lo = hexval(s[2 * i + 1]);
        if (hi < 0 || lo < 0) return 0;
        out[i] = (unsigned char)((hi << 4) | lo);
    }
    return 1;
}

static void bytes2hex(const unsigned char *in, char *out, int n)
{
    static const char *H = "0123456789ABCDEF";
    int i;
    for (i = 0; i < n; i++) {
        out[2 * i]     = H[in[i] >> 4];
        out[2 * i + 1] = H[in[i] & 0x0F];
    }
    out[2 * n] = '\0';
}

int main(void)
{
    char keyhex[64], pthex[64], hex[65];
    unsigned char key[16], pt[16], out[16], rk[176];
    int choice;

    printf("========== AES-128 (Advanced Encryption Standard) ==========\n");
    printf("Enter 16-byte key  (32 hex chars) : ");
    scanf("%32s", keyhex);
    printf("Enter 16-byte block(32 hex chars): ");
    scanf("%32s", pthex);

    if (!hex2bytes(keyhex, key, 16) || !hex2bytes(pthex, pt, 16)) {
        printf("Invalid hex input!\n");
        return 1;
    }

    key_expansion(key, rk);

    printf("1. Encrypt  2. Decrypt : ");
    scanf("%d", &choice);

    if (choice == 1) {
        aes_encrypt(pt, out, rk);
        bytes2hex(out, hex, 16);
        printf("Cipher text : %s\n", hex);
    } else {
        aes_decrypt(pt, out, rk);
        bytes2hex(out, hex, 16);
        printf("Plain text  : %s\n", hex);
    }

    /* round-trip self check */
    {
        unsigned char ct[16], back[16];
        aes_encrypt(pt, ct, rk);
        aes_decrypt(ct, back, rk);
        if (memcmp(back, pt, 16) == 0)
            printf("== Round-trip check OK ==\n");
        else
            printf("== Round-trip check FAILED ==\n");
    }

    return 0;
}