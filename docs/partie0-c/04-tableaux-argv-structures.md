# 0.4 Tableaux, arguments et structures

Ce dernier chapitre de la partie 0 rassemble ce qui revient le plus souvent dans un crackme : lire les arguments passés au programme, convertir une chaîne en nombre, et regrouper des données dans une structure. Ce sont les formes qu'on retrouvera désassemblées dans les premiers défis de reverse.

## Les tableaux

Un tableau est une suite de cases du même type, rangées côte à côte en mémoire :

```c
int tab[5] = {10, 20, 30, 40, 50};
printf("%d\n", tab[0]);   // 10 (les indices commencent à 0)
printf("%d\n", tab[4]);   // 50
```

Le nom d'un tableau se comporte presque comme un pointeur vers sa première case : `tab[i]` est `*(tab + i)`. Il n'y a aucun contrôle des bornes en C : lire `tab[5]` ou plus loin ne provoque pas d'erreur immédiate, mais lit de la mémoire quelconque. C'est la base d'une grande partie des vulnérabilités (chapitre 17).

## Les arguments de la ligne de commande

`main` peut recevoir les arguments passés au programme :

```c
int main(int argc, char **argv)
{
    // argc : nombre d'arguments (le nom du programme compte pour 1)
    // argv : tableau de chaînes ; argv[0] est le nom du programme
    for (int i = 0; i < argc; i++)
        my_putstr_endl(argv[i]);
    return 0;
}
```

`char **argv` est un pointeur vers des pointeurs de caractères : un tableau de chaînes. Si on lance `./prog bonjour 42`, alors `argc` vaut 3, `argv[0]` est `"./prog"`, `argv[1]` est `"bonjour"`, `argv[2]` est `"42"`. Presque tous les crackmes attendent leur mot de passe en argument : savoir lire `argv` est indispensable.

## Convertir une chaîne en nombre

Un argument arrive toujours sous forme de chaîne. `"42"` (les caractères `'4'` et `'2'`) n'est pas le nombre `42`. Convertir revient à parcourir les chiffres et à accumuler :

```c
// principe : resultat = resultat * 10 + (chiffre courant)
```

C'est exactement ce que fait `atoi` de la libc. L'écrire soi-même (exercice ci-dessous) permet de reconnaître ce motif de multiplication par 10 quand on le croisera en assembleur.

## Les structures

Une **structure** regroupe plusieurs valeurs sous un seul nom :

```c
struct point
{
    int x;
    int y;
};
```

On accède aux champs avec `.` sur une variable, ou `->` sur un pointeur :

```c
struct point a = {1, 2};
printf("%d %d\n", a.x, a.y);

struct point *p = &a;
printf("%d\n", p->x);     // p->x est (*p).x
```

En mémoire, les champs sont rangés les uns après les autres. En reverse, une structure se repère justement à ces accès réguliers à des offsets fixes à partir d'une même adresse de base (`[rdi]`, `[rdi+4]`...), comme on le verra au chapitre 6.

## Exercices

**Exercice 0.4.1** Écrire `int somme_tableau(int *tab, int taille)` qui renvoie la somme des `taille` premiers éléments d'un tableau d'entiers.

**Exercice 0.4.2** Écrire un programme qui affiche ses arguments, un par ligne, en commençant par `argv[1]` (sans afficher le nom du programme). Le tester avec `./prog un deux trois`.

**Exercice 0.4.3** Écrire `int my_getnbr(const char *s)` qui convertit une chaîne en entier. On ignore d'éventuels espaces en tête, puis on lit une suite de signes `+` et `-` : plusieurs signes peuvent se suivre, et **chaque `-` inverse le résultat** (deux `-` s'annulent). On lit ensuite les chiffres et on s'arrête au premier caractère non numérique. Exemples attendus : `"42"` donne 42, `"  -42abc"` donne -42, `"+-+-13"` donne 13 (deux `-`, qui s'annulent).

**Exercice 0.4.4** Définir `struct point { int x; int y; };` puis écrire `int distance_carre(struct point *a, struct point *b)` qui renvoie le carré de la distance entre deux points, soit `(ax-bx)^2 + (ay-by)^2`. On évite la racine carrée pour rester en entiers. Vérifier : entre `{1, 2}` et `{4, 6}`, le résultat est 25.

**Exercice 0.4.5** (bonus, fait le lien avec la suite) Écrire un mini-crackme : un programme qui attend un nombre en argument, le convertit avec `my_getnbr`, et affiche `Gagne` si ce nombre vaut `1337`, `Perdu` sinon. C'est la structure exacte des premiers défis de la partie 2, cette fois côté source.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-04)

## Quelques fonctions de la libc

Jusqu'ici on a tout réécrit à la main, pour comprendre. En pratique, la bibliothèque standard (la libc) fournit ces fonctions toutes faites. Les crackmes de la partie 2 les utilisent beaucoup, en particulier pour lire une saisie et comparer des chaînes. Les plus utiles :

| Fonction | En-tête | Rôle |
|---|---|---|
| `strlen(s)` | `string.h` | longueur d'une chaîne (ce que fait `my_strlen`) |
| `strcmp(a, b)` | `string.h` | compare deux chaînes ; renvoie 0 si elles sont égales |
| `strncmp(a, b, n)` | `string.h` | compare les `n` premiers caractères |
| `atoi(s)` | `stdlib.h` | convertit une chaîne en entier (ce que fait `my_getnbr`) |
| `fgets(buf, taille, stdin)` | `stdio.h` | lit une ligne saisie au clavier dans `buf` |
| `malloc(n)` / `free(p)` | `stdlib.h` | réserve / libère `n` octets de mémoire |

Un programme qui demande un mot de passe ressemble alors à ceci, et c'est exactement la forme des coffres de la partie 2 :

```c
#include <stdio.h>
#include <string.h>

int main(void)
{
    char saisie[64];

    printf("Mot de passe : ");
    fgets(saisie, sizeof(saisie), stdin);   // lit la ligne
    saisie[strcspn(saisie, "\n")] = '\0';    // retire le retour à la ligne final

    if (strcmp(saisie, "ouvre-toi") == 0)
        printf("Acces autorise\n");
    else
        printf("Refuse\n");
    return 0;
}
```

Le détail de chaque fonction est dans son manuel : `man 3 strcmp`, `man 3 fgets`, etc. (la section 3 est celle des fonctions de la libc).

## Et ensuite

Vous savez maintenant lire du C : fonctions, variables, conditions, boucles, pointeurs, chaînes, tableaux, arguments et structures. C'est tout ce qu'il faut pour aborder l'assembleur, car l'assembleur n'est que la forme compilée de ces mêmes idées. La [partie 1](../partie1-assembleur/01-bases.md) reprend chacune de ces constructions et montre à quoi elle ressemble une fois traduite par le compilateur.

## Suivi

- [ ] Chapitre 0.4 : lecture
- [ ] Chapitre 0.4 : exercices
