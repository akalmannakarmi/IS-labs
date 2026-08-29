/* ============================================================
   Lab 11 : Role Based Access Control (RBAC)
   ------------------------------------------------------------
   Users are mapped to roles; roles are mapped to permissions.
   Permissions: read, write, delete, execute.
     Admin  -> all permissions
     User   -> read + write
     Guest  -> read only
   The authorization check simply looks up a permission matrix.
   ============================================================ */
#include <stdio.h>
#include <string.h>

#define NUM_ROLES 3
#define NUM_OPS   4

/* permission matrix: rows = roles, cols = operations */
static const char *perm[NUM_ROLES][NUM_OPS] = {
    {"read",   "write", "delete",  "execute"},   /* Admin */
    {"read",   "write", "denied",  "denied"},    /* User  */
    {"read",   "denied","denied",  "denied"}     /* Guest */
};

static const char *roles[NUM_ROLES] = {"Admin", "User", "Guest"};
static const char *ops[NUM_OPS]     = {"Read", "Write", "Delete", "Execute"};

int main(void)
{
    int role, op, i;

    printf("========== ROLE BASED ACCESS CONTROL ==========\n");
    printf("Choose your role :\n");
    for (i = 0; i < NUM_ROLES; i++)
        printf("   %d. %s\n", i + 1, roles[i]);
    printf("Role : ");
    scanf("%d", &role);
    if (role < 1 || role > NUM_ROLES) {
        printf("Invalid role.\n");
        return 1;
    }
    role--;

    printf("\nLogged in as : %s\n", roles[role]);
    printf("Allowed permissions :");
    for (i = 0; i < NUM_OPS; i++)
        if (strcmp(perm[role][i], "denied") != 0)
            printf(" %s", ops[i]);
    printf("\n");

    printf("\nAvailable operations :\n");
    for (i = 0; i < NUM_OPS; i++)
        printf("   %d. %s\n", i + 1, ops[i]);
    printf("Operation : ");
    scanf("%d", &op);
    if (op < 1 || op > NUM_OPS) {
        printf("Invalid operation.\n");
        return 1;
    }
    op--;

    /* ---------- the actual authorization decision ---------- */
    if (strcmp(perm[role][op], "denied") == 0)
        printf("\nACCESS DENIED : %s is not allowed for %s.\n",
               ops[op], roles[role]);
    else
        printf("\nACCESS GRANTED : %s can %s.\n", roles[role], ops[op]);

    return 0;
}