/* Coffre v1 : premier contact (chapitre 8).
 * Compilation : gcc -O0 -o coffre_v1 coffre_v1.c
 * Regle du jeu : ne pas lire ce source avant d'avoir trouve le mot de passe
 * avec les outils du chapitre. Le compiler sans l'ouvrir (voir le README).
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
    char saisie[64];

    puts("=== Coffre v1 ===");
    printf("Mot de passe : ");
    if (fgets(saisie, sizeof(saisie), stdin) == NULL)
        return 1;
    saisie[strcspn(saisie, "\n")] = 0;

    if (strcmp(saisie, "Sesame_ouvre_toi") == 0)
        puts("Coffre ouvert. FLAG{v1_strings_suffit}");
    else
        puts("Acces refuse.");
    return 0;
}
