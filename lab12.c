/* ============================================================
   Lab 12 : Malicious Logic (self-replicating "virus" demo)
   ------------------------------------------------------------
   A real virus silently copies its own code into other files
   and runs a payload. Here we simulate that behaviour safely:
     1) reads the running executable itself (argv[0])
     2) writes an exact byte-copy of it to a target file
        -> demonstrating *self replication*
     3) runs a harmless "payload" that logs a marker file
   This is a study-only simulation; nothing destructive runs.
   Binary mode ("rb"/"wb") keeps byte-copies exact on Windows.
   ============================================================ */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    const char *target = "infected_copy";
    FILE *self, *clone;
    int ch;

    printf("========== MALICIOUS LOGIC (VIRUS SIMULATION) ==========\n");

    if (argc > 1)
        target = argv[1];                /* optional target file name   */

    /* -------- 1) open our own executable in binary mode --------- */
    self = fopen(argv[0], "rb");
    if (self == NULL) {
        printf("Cannot open the running program.\n");
        return 1;
    }

    clone = fopen(target, "wb");
    if (clone == NULL) {
        printf("Cannot create the target file.\n");
        fclose(self);
        return 1;
    }

    /* -------- 2) replicate: copy every byte of ourselves --------- */
    while ((ch = fgetc(self)) != EOF)
        fputc(ch, clone);

    {
        long size = ftell(clone);        /* bytes written         */
        fclose(clone);
        fclose(self);
        printf("[*] Replicated executable into '%s' (%ld bytes copied).\n",
               target, size);
    }

    /* -------- 3) simulated payload (harmless) -------------------- */
    {
        FILE *marker = fopen("payload_log.txt", "w");
        if (marker) {
            fprintf(marker, "Payload executed at... (simulation only)\n");
            fclose(marker);
            printf("[*] Payload ran: wrote 'payload_log.txt'.\n");
        }
    }

    printf("\nNote: this is an educational simulation of how viruses\n");
    printf("self-replicate; removing the infected copy left behind:\n");
    printf("  %s\n", target);
    return 0;
}