# Chapitre 11 : Allocation dynamique

Jusqu'ici, la taille des tableaux était fixée à l'écriture du programme. L'allocation dynamique permet de réserver de la mémoire dont la taille n'est connue qu'à l'exécution (une saisie, un fichier). C'est aussi la source de bugs très étudiés en reverse et en exploitation (fuites, double libération, dépassement de tas).

## malloc et free

`malloc(n)` réserve `n` octets et renvoie leur adresse (ou `NULL` en cas d'échec). `free(p)` rend cette mémoire. Tout `malloc` doit avoir son `free`.

```c
#include <stdlib.h>

int *t = malloc(5 * sizeof(int));   // un tableau de 5 int
if (t == NULL)
    return 1;                       // toujours vérifier
for (int i = 0; i < 5; i++)
    t[i] = i * i;
// ... utilisation ...
free(t);                            // libérer quand on a fini
```

On calcule la taille avec `sizeof` pour rester correct quel que soit le type. Une fois `free(t)` appelé, `t` ne doit plus être utilisé.

## Fuites et pièges

- **Fuite mémoire** : un `malloc` sans `free`. Le programme consomme de plus en plus de mémoire.
- **Double free** : appeler `free` deux fois sur la même adresse. Comportement indéfini, souvent exploitable.
- **Usage après libération** (*use after free*) : lire ou écrire via un pointeur déjà libéré.

Ces trois défauts reviennent constamment dans les challenges d'exploitation de tas ; les voir en C aide à les repérer plus tard.

## Dupliquer une chaîne

Un usage très courant : faire une copie modifiable d'une chaîne, en allouant juste ce qu'il faut.

```c
char *my_strdup(const char *s)
{
    int n = my_strlen(s);
    char *d = malloc(n + 1);        // +1 pour le '\0'
    if (d == NULL) return NULL;
    for (int i = 0; i <= n; i++)    // <= pour copier aussi le '\0'
        d[i] = s[i];
    return d;
}
```

## Exercices

!!! warning "Contraintes"
    `strdup` et les fonctions de `<string.h>` restent interdites. Chaque `malloc` doit avoir son `free` ; vérifiez vos programmes, par exemple avec `valgrind` dans le conteneur.

**Exercice 11.1 (N1)** Écrire `int *tableau_rempli(int n)` qui alloue un tableau de `n` entiers, le remplit avec les carrés (`t[i] = i * i`), et le renvoie. Dans le `main`, l'utiliser puis le libérer.

**Exercice 11.2 (N2)** Écrire `char *my_strdup(const char *s)` qui renvoie une copie allouée de `s`. Ne pas oublier le `'\0'` ni le `free` dans l'appelant.

**Exercice 11.3 (N2)** Écrire `char *concat(const char *a, const char *b)` qui renvoie une nouvelle chaîne allouée contenant `a` suivi de `b`. `concat("Cle", "mence")` donne `"Clemence"`.

**Exercice 11.4 (N3)** Écrire `int *copie_agrandie(const int *tab, int n)` qui renvoie un nouveau tableau de taille `n + 1`, copie de `tab`, avec une case supplémentaire à la fin valant la somme des `n` premières. Libérer l'ancien si besoin dans l'appelant.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-11)

## Fil rouge : copie défensive de la clé

Avant de vérifier la clé, on en fait une copie allouée avec `my_strdup`. Ici ce n'est pas indispensable, mais c'est un réflexe utile : travailler sur une copie plutôt que sur la donnée reçue, puis libérer.

```c
char *copie = my_strdup(argv[1]);
if (copie == NULL)
    return 1;
int ouvert = verifier(&coffre, copie);
free(copie);
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-11)

## Suivi

- [ ] Partie 1, chapitre 11 : lecture
- [ ] Partie 1, chapitre 11 : exercices
- [ ] Partie 1, chapitre 11 : fil rouge
