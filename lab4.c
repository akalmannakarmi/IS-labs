/* ============================================================
   Lab 4 : DES - Data Encryption Standard (full 16 rounds)
   ------------------------------------------------------------
   All the big lookup tables live in des_tables.h; this file
   contains only the core Feistel-network logic.
     * Key schedule  : PC-1, split into C/D, rotate, PC-2
     * Round function: expansion E, XOR sub-key, 8 S-boxes, P
   Inputs/outputs are 8-byte blocks shown as 16 hex characters.
   ============================================================ */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "des_tables.h"

/* ------- generic permutation: applies 1-based table to a value */
static uint64_t permute(uint64_t in, const int *tb, int n)
{
    uint64_t out = 0;
    int i;
    for (i = 0; i < n; i++)
        out = (out << 1) | ((in >> (64 - tb[i])) & 1ULL);
    return out;
}

/* ------- derive the 48-bit subkey for one round ------------------ */
static uint64_t des_subkey(uint64_t key, int round)
{
    uint64_t key56 = permute(key, PC1, 56);   /* drop parity bits */
    uint32_t C = (uint32_t)(key56 >> 28);     /* left  half  (28) */
    uint32_t D = (uint32_t)(key56 & 0x0FFFFFFF); /* right half (28) */
    uint32_t mask = 0x0FFFFFFF;
    int k;

    /* cumulative left rotation through round `round` */
    for (k = 0; k <= round; k++) {
        int s = SHIFTS[k];
        C = ((C << s) | (C >> (28 - s))) & mask;
        D = ((D << s) | (D >> (28 - s))) & mask;
    }

    /* merge C|D and compress to 48 bits with PC-2 */
    {
        uint64_t cd = ((uint64_t)C << 28) | D, out = 0;
        int i;
        for (i = 0; i < 48; i++)
            out = (out << 1) | ((cd >> (56 - PC2[i])) & 1ULL);
        return out;
    }
}

/* ------- the round function f(R, K) ------------------------------ */
static uint32_t feistel(uint32_t r, uint64_t k48)
{
    uint64_t e = 0;      /* expanded 48 bits */
    uint32_t sout = 0, p = 0;
    int i;

    for (i = 0; i < 48; i++)
        e = (e << 1) | ((r >> (32 - E[i])) & 1ULL);
    e ^= k48;

    /* 8 groups of 6 bits -> 8 groups of 4 bits via S-boxes */
    for (i = 0; i < 8; i++) {
        int b   = (int)((e >> (42 - 6 * i)) & 63);
        int row = ((b >> 4) & 2) | (b & 1);   /* bit1 + bit6  */
        int col = (b >> 1) & 15;              /* bits 2..5    */
        sout = (sout << 4) | (uint32_t)SBOX[i][row][col];
    }

    /* straight permutation P */
    for (i = 0; i < 32; i++)
        p = (p << 1) | ((sout >> (32 - P[i])) & 1U);
    return p;
}

/* ------- DES on one 64-bit block -------------------------------- */
static uint64_t des_block(uint64_t block, uint64_t key, int decrypt)
{
    uint64_t ip = permute(block, IP, 64);
    uint32_t L = (uint32_t)(ip >> 32), R = (uint32_t)(ip & 0xFFFFFFFF);
    int rnd;

    for (rnd = 0; rnd < 16; rnd++) {
        int round = decrypt ? 15 - rnd : rnd;   /* reverse key order */
        uint32_t oldR = R;
        R = L ^ feistel(R, des_subkey(key, round));
        L = oldR;
    }

    /* swap L and R before the final permutation */
    return permute(((uint64_t)R << 32) | L, FP, 64);
}

/* ------- helpers for hex <-> bytes ------------------------------- */
static int hexval(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int hex2bytes(const char *s, uint64_t *out)
{
    int i;
    *out = 0;
    for (i = 0; i < 16; i++) {
        int v = hexval(s[i]);
        if (v < 0) return 0;
        *out = (*out << 4) | (uint64_t)v;
    }
    return 1;
}

static void bytes2hex(uint64_t in, char *out)
{
    static const char *H = "0123456789ABCDEF";
    int i;
    for (i = 0; i < 16; i++)
        out[i] = H[(int)((in >> (60 - 4 * i)) & 0xF)];
    out[16] = '\0';
}

int main(void)
{
    char keyhex[32], pthex[32], hex[17];
    uint64_t key, pt, ct, back;
    int choice;

    printf("========== DES (Data Encryption Standard) ==========\n");
    printf("Enter 8-byte key  (16 hex chars)  : ");
    scanf("%16s", keyhex);
    printf("Enter 8-byte block(16 hex chars)  : ");
    scanf("%16s", pthex);

    if (!hex2bytes(keyhex, &key) || !hex2bytes(pthex, &pt)) {
        printf("Invalid hex input!\n");
        return 1;
    }

    printf("1. Encrypt  2. Decrypt : ");
    scanf("%d", &choice);

    if (choice == 1) {
        ct   = des_block(pt, key, 0);
        bytes2hex(ct, hex);
        printf("Cipher text : %s\n", hex);
    } else {
        ct   = des_block(pt, key, 1);
        bytes2hex(ct, hex);
        printf("Plain text  : %s\n", hex);
    }

    /* round-trip self check keeps the report honest */
    back = des_block(des_block(pt, key, 0), key, 1);
    if (back == pt)
        printf("== Round-trip check OK ==\n");
    else
        printf("== Round-trip check FAILED ==\n");

    return 0;
}