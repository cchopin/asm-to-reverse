#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
/* "Oignon" : plusieurs couches. Anti-debug ptrace, puis le mot de passe
   attendu est reconstruit par deux XOR successifs a partir de deux tableaux.
   gcc -O1 -fno-stack-protector -no-pie oignon.c -o oignon */

static unsigned char couche_a[] = {   /* couche 1 */
    0x3B,0x1E,0x0A,0x11,0x0B,0x1C,0x56,0x4C,0x0C,0x12,0x16
};
static unsigned char couche_b[] = {   /* couche 2 */
    0x4B,0x7B,0x66,0x64,0x79,0x79,0x09,0x7D,0x3F,0x21,0x21
};

int main(int argc, char **argv)
{
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        printf("Nope.\n");           /* debugger detecte */
        return 1;
    }
    if (argc != 2) { printf("usage: %s <mot-de-passe>\n", argv[0]); return 2; }
    size_t n = sizeof(couche_a);
    char attendu[32];
    for (size_t i = 0; i < n; i++)
        attendu[i] = couche_a[i] ^ couche_b[i];   /* le vrai mot de passe */
    attendu[n] = 0;
    if (strlen(argv[1]) == n && memcmp(argv[1], attendu, n) == 0)
        printf("FLAG{oignon_%s}\n", attendu);
    else
        printf("Refuse\n");
    return 0;
}
