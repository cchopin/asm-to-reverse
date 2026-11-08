/* Fil rouge partie 1.
 * NE PAS LIRE CE FICHIER avant le chapitre 7 de la partie 2 : le but est de retrouver
 * ce code uniquement a partir du desassemblage (mystere_O0.txt).
 *
 * Compilation : gcc -O0 -fno-stack-protector -no-pie -o mystere mystere.c
 */
#include <stdio.h>

int verifier(const char *s)
{
    int len = 0;
    while (s[len] != 0)
        len++;
    if (len != 8)
        return 0;

    int somme = 0;
    for (int i = 0; i < 8; i++) {
        if (s[i] < '0' || s[i] > '9')
            return 0;
        somme += s[i] - '0';
    }
    if (somme != 40)
        return 0;
    if (s[0] != s[7])
        return 0;
    return 1;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage : %s <code>\n", argv[0]);
        return 1;
    }
    if (verifier(argv[1]))
        puts("Code accepte");
    else
        puts("Code refuse");
    return 0;
}
