/* Coffre v3 : un nom, un numero de serie, un algorithme (chapitre 11).
 * Compilation : gcc -O1 -o coffre_v3 coffre_v3.c
 * Objectif : ecrire un keygen, c'est a dire un programme qui calcule
 * le bon serial pour n'importe quel nom.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int hacher(const char *nom)
{
    unsigned int h = 0x1337;
    for (size_t i = 0; nom[i] != 0; i++) {
        h = h * 31 + (unsigned char)nom[i];
        h = (h << 3) | (h >> 29);      /* rotation a gauche de 3 bits */
    }
    return h ^ 0xC0FFEE;
}

static void lire(const char *invite, char *buf, size_t taille)
{
    printf("%s", invite);
    if (fgets(buf, (int)taille, stdin) == NULL)
        exit(1);
    buf[strcspn(buf, "\n")] = 0;
}

int main(void)
{
    char nom[64], serial[64];

    puts("=== Coffre v3 ===");
    lire("Nom : ", nom, sizeof(nom));
    lire("Serial : ", serial, sizeof(serial));

    if (strlen(nom) < 4) {
        puts("Nom trop court.");
        return 1;
    }

    unsigned int attendu = hacher(nom);
    unsigned int donne = (unsigned int)strtoul(serial, NULL, 16);

    if (attendu == donne)
        printf("Coffre ouvert pour %s. FLAG{keygen_%08x}\n", nom, attendu);
    else
        puts("Serial invalide.");
    return 0;
}
