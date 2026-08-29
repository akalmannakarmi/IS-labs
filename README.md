# IS Lab Programs

12 Information Security lab programs + 3 table headers.
All programs are plain standard C (C99) and compile identically on
Linux, macOS and Windows (MinGW/gcc or Visual Studio gcc).

## Compiling

```bash
gcc -o lab1  lab1.c
gcc -o lab2  lab2.c
# ... and so on for lab3 ... lab12
```

Compile one-time check for everything:

```bash
for i in 1 2 3 4 5 6 7 8 9 10 11 12; do gcc -o lab$i lab$i.c; done
```

On Windows with MinGW: `gcc lab1.c -o lab1.exe`

---

## lab1.c - Caesar Cipher

Shift cipher: every letter is moved `key` places in the alphabet.

Run: `./lab1`   (Windows: `lab1.exe`)

```
Enter text       : Hello World
Enter key (1-25) : 3
1. Encrypt  2. Decrypt : 1
Result           : Khoor Zruog
```

- Works on letters only; spaces/digits stay unchanged.
- Decrypt uses key `26-key`.

---

## lab2.c - Playfair Cipher

5x5 digraph cipher with a keyword, I/J merged into one cell.
X is inserted between doubled letters and as padding.

Run: `./lab2`

```
Enter keyword : MONARCHY
Enter message : HELLO
1. Encrypt  2. Decrypt : 1
Result        : ...
```

Decryption is the inverse: same keyword, paste the cipher, choose 2.
A trailing padding X is removed automatically.

---

## lab3.c - Rail Fence Cipher

Transposition cipher over `N` rails in a zig-zag.

Run: `./lab3`

```
Enter text      : HIDETHEGOLD
Enter rails(2+) : 3
Cipher text    : HTOIEHGLDED
Decrypted text : HIDETHEGOLD
```

The program prints both cipher and recovered text and checks the
round-trip for you. Rails must be between 2 and 8.

---

## lab4.c - DES (Data Encryption Standard)

Full 16-round DES. Tables are in `des_tables.h` (automatically
included - compile only `lab4.c`). Input/output are 8-byte
blocks written as 16 hex characters.

Run: `./lab4`

```
Enter 8-byte key  (16 hex chars)  : 133457799BBCDFF1
Enter 8-byte block(16 hex chars)  : 0123456789ABCDEF
1. Encrypt  2. Decrypt : 1
Cipher text : 85E813540F0AB405
```

This is the official FIPS test vector - use it to verify.
A round-trip self-check is printed automatically.

---

## lab5.c - AES (Advanced Encryption Standard)

Full AES-128 (FIPS-197). Tables in `aes_tables.h` (included
automatically, compile only `lab5.c`). 16-byte key and block,
each as 32 hex characters.

Run: `./lab5`

```
Enter 16-byte key  (32 hex chars) : 000102030405060708090a0b0c0d0e0f
Enter 16-byte block(32 hex chars): 00112233445566778899aabbccddeeff
1. Encrypt  2. Decrypt : 1
Cipher text : 69C4E0D86A7B0430D8CDB78070B4C55A
```

Official FIPS-197 test vector - use it to verify.

---

## lab6.c - Miller-Rabin Primality Testing

Probabilistic primality test. Uses bases {2,3,5,7,11,13} which
make it deterministic for all numbers < 3.4e14.

Run: `./lab6`

```
Enter a number : 104729
104729 is PRIME (probably prime - deterministic for this range)
```

Try: `104729` (prime), `561` (composite), `97` (prime).

---

## lab7.c - Euclidean Algorithm

Menu-driven: basic GCD, extended GCD (finds x,y with
`a*x + b*y = gcd`), or modular inverse of `a mod b` (the RSA
building block).

Run: `./lab7`

```
Enter a = 252
Enter b = 105
1. GCD only
2. GCD + extended (a*x + b*y = gcd)
3. Modular inverse of a mod b
Choice : 2
gcd(252, 105) = 21
Coefficients : x = -2, y = 5
Check : 252*-2 + 105*5 = 21
```

---

## lab8.c - RSA (Rivest-Shamir-Adleman)

Character-by-character RSA. Enter two small primes and a public
exponent; the program prints the keys, encrypts, then decrypts.

Run: `./lab8`

```
Enter prime p         : 61
Enter prime q         : 53
Enter exponent e      : 17
Public key  (e, n) = (17, 3233)
Private key (d, n) = (2753, 3233)
Enter message       : HELLO
Encrypted blocks...
Decrypted text (M = C^d mod n)  : HELLO
```

Use small primes (e.g. 61 and 53) for clean 16-bit-long blocks.
If `e` has no inverse for your primes, pick another exponent.

---

## lab9.c - MD5 Message Digest

Real MD5 (RFC 1321). Tables in `md5_tables.h` (included
automatically, compile only `lab9.c`).

Run: `./lab9`

```
Enter message : abc
MD5 digest    : 900150983cd24fb0d6963f7d28e17f72
```

Official RFC 1321 vector - use it to verify.

---

## lab10.c - Authentication (Password + CAPTCHA)

Two-step login used by many real systems:

1. Password (default is `tu@123`, 3 attempts)
2. CAPTCHA (random 4-6 letter/digit code, 3 attempts)

Run: `./lab10`

```
Password : tu@123
CAPTCHA  : H9GP7H
Type it  : <type what you see>
====== AUTHENTICATED! Access GRANTED ======
```

Types the CAPTCHA shown by the program - it changes every run.

---

## lab11.c - Role Based Access Control (RBAC)

A permission matrix maps roles to operations:

| Role  | Read | Write | Delete | Execute |
|-------|------|-------|--------|---------|
| Admin | .    | .     | .      | .       |
| User  | .    | .     | denied | denied  |
| Guest | .    | denied| denied | denied  |

Run: `./lab11`

```
Role : 2           (1=Admin 2=User 3=Guest)
Operation : 4      (1=Read 2=Write 3=Delete 4=Execute)
ACCESS DENIED : Execute is not allowed for User.
```

---

## lab12.c - Malicious Logic (Virus Simulation)

Educational simulation of a self-replicating virus: the running
program copies its own executable and runs a harmless payload.

Run: `./lab12`                 (copies itself to `infected_copy`)
Run with a name: `./lab12 myclone`   (copies to `myclone`)

```
[*] Replicated executable into 'myclone' (16256 bytes copied).
[*] Payload ran: wrote 'payload_log.txt'.
```

Clean up after the demo:

```bash
rm -f infected_copy payload_log.txt
```

On Windows run `lab12.exe evilcopy.exe` and delete the files
afterwards. This is a study-only simulation - nothing destructive.

---

## Notes for the lab report / teacher

- lab4, lab5, lab9 match official standard test vectors, so you can
  honestly state "verified against the published test vector".
- lab4.security: DES/AES/MD5 are included as academic examples;
  DES and MD5 are outdated for production use (AES-256 / SHA-2+).
- Miller-Rabin is probabilistic in general; the small fixed bases
  make it exact for the range stated in lab6.
- Everything uses only standard C headers (stdio, stdlib, string,
  ctype, time, stdint) so it builds on Linux, macOS and Windows
  without changes.