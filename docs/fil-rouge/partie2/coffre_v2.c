/* Coffre v2 : le mot de passe n'est plus en clair (chapitres 9 et 10).
 * Compilation : gcc -O0 -o coffre_v2 coffre_v2.c
 */
#include <stdio.h>
#include <string.h>

/* "R3v3rs3_m3_1f_u_c4n" chiffre octet par octet avec XOR 0x2A */
static const unsigned char cle[] = {
    0x78, 0x19, 0x5c, 0x19, 0x58, 0x59, 0x19, 0x75, 0x47, 0x19,
    0x75, 0x1b, 0x4c, 0x75, 0x5f, 0x75, 0x49, 0x1e, 0x44
};

static int verifier(const char *s)
{
    if (strlen(s) != sizeof(cle))
        return 0;
    for (size_t i = 0; i < sizeof(cle); i++) {
        if ((unsigned char)(s[i] ^ 0x2A) != cle[i])
            return 0;
    }
    return 1;
}

int main(void)
{
    char saisie[64];

    puts("=== Coffre v2 ===");
    printf("Mot de passe : ");
    if (fgets(saisie, sizeof(saisie), stdin) == NULL)
        return 1;
    saisie[strcspn(saisie, "\n")] = 0;

    if (verifier(saisie))
        printf("Coffre ouvert. FLAG{%s}\n", saisie);
    else
        puts("Acces refuse.");
    return 0;
}
