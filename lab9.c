/* ============================================================
   Lab 9 : MD5 - MD5 Message-Digest Algorithm (RFC 1321)
   ------------------------------------------------------------
   Only the K and shift tables live in md5_tables.h; this file
   holds the core logic:
     * 4 chaining values (A,B,C,D) initialised to fixed IVs
     * message padded  -> append 0x80, zeros, 64-bit length
     * 4 rounds x 16 steps of the compression function
     * little-endian 32-bit word handling (portable, manual load)
   ============================================================ */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "md5_tables.h"

typedef uint32_t u32;
typedef uint64_t u64;

/* left rotate x by c bits */
static u32 rotl(u32 x, int c)
{
    return (x << c) | (x >> (32 - c));
}

/* load a little-endian 32-bit word, byte by byte (portable) */
static u32 le32(const unsigned char *p)
{
    return (u32)p[0] | ((u32)p[1] << 8) |
           ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

/* one 64-byte block of the MD5 compression function */
static void md5_block(u32 h[4], const unsigned char buf[64])
{
    u32 m[16], a, b, c, d;
    int i;

    for (i = 0; i < 16; i++)
        m[i] = le32(buf + 4 * i);

    a = h[0]; b = h[1]; c = h[2]; d = h[3];

    for (i = 0; i < 64; i++) {
        u32 f;
        int g;
        if (i < 16)      { f = (b & c) | (~b & d);       g = i;            }
        else if (i < 32) { f = (d & b) | (~d & c);       g = (5 * i + 1) & 15; }
        else if (i < 48) { f = b ^ c ^ d;                g = (3 * i + 5) & 15; }
        else             { f = c ^ (b | ~d);             g = (7 * i) & 15; }
        {
            u32 tmp = d;
            d = c;
            c = b;
            b = b + rotl(a + f + K[i] + m[g], S[i]);
            a = tmp;
        }
    }

    h[0] += a; h[1] += b; h[2] += c; h[3] += d;
}

/* whole-message hashing entry point */
static void md5_hash(const unsigned char *msg, size_t len,
                     unsigned char digest[16])
{
    u32 h[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    unsigned char block[64], last[64];
    u64 bits;
    size_t i, rem;

    /* process every complete 64-byte block */
    for (i = 0; i + 64 <= len; i += 64)
        md5_block(h, msg + i);

    /* --- padding: 0x80, zeros, then 64-bit bit-length (little endian) --- */
    rem = len % 64;
    memcpy(block, msg + len - rem, rem);
    block[rem] = 0x80;
    bits = (u64)len * 8;

    if (rem < 56) {                 /* padding fits in this block */
        memset(block + rem + 1, 0, 56 - rem - 1);
        block[56] = (unsigned char)(bits & 0xFF);
        block[57] = (unsigned char)((bits >> 8) & 0xFF);
        block[58] = (unsigned char)((bits >> 16) & 0xFF);
        block[59] = (unsigned char)((bits >> 24) & 0xFF);
        block[60] = (unsigned char)((bits >> 32) & 0xFF);
        block[61] = (unsigned char)((bits >> 40) & 0xFF);
        block[62] = (unsigned char)((bits >> 48) & 0xFF);
        block[63] = (unsigned char)((bits >> 56) & 0xFF);
        md5_block(h, block);
    } else {                        /* need a second, all-padding block */
        memset(block + rem + 1, 0, 64 - rem - 1);
        md5_block(h, block);

        memset(last, 0, sizeof last);
        last[56] = (unsigned char)(bits & 0xFF);
        last[57] = (unsigned char)((bits >> 8) & 0xFF);
        last[58] = (unsigned char)((bits >> 16) & 0xFF);
        last[59] = (unsigned char)((bits >> 24) & 0xFF);
        last[60] = (unsigned char)((bits >> 32) & 0xFF);
        last[61] = (unsigned char)((bits >> 40) & 0xFF);
        last[62] = (unsigned char)((bits >> 48) & 0xFF);
        last[63] = (unsigned char)((bits >> 56) & 0xFF);
        md5_block(h, last);
    }

    /* --- write A,B,C,D out, least significant byte first --- */
    for (i = 0; i < 4; i++) {
        digest[4 * i + 0] = (unsigned char)(h[i] & 0xFF);
        digest[4 * i + 1] = (unsigned char)((h[i] >> 8) & 0xFF);
        digest[4 * i + 2] = (unsigned char)((h[i] >> 16) & 0xFF);
        digest[4 * i + 3] = (unsigned char)((h[i] >> 24) & 0xFF);
    }
}

int main(void)
{
    char msg[512];
    unsigned char digest[16];
    size_t len;
    int i;

    printf("========== MD5 MESSAGE DIGEST ==========\n");
    printf("Enter message : ");
    fgets(msg, sizeof msg, stdin);
    msg[strcspn(msg, "\n")] = '\0';
    len = strlen(msg);

    md5_hash((const unsigned char *)msg, len, digest);

    printf("MD5 digest    : ");
    for (i = 0; i < 16; i++)
        printf("%02x", digest[i]);
    printf("\n");
    return 0;
}