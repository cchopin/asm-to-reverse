# 0.3 Pointeurs et chaînes

C'est le chapitre le plus important de cette partie 0 pour le reverse. Un pointeur est une adresse, et en assembleur presque tout est adresse : les registres qui pointent vers la pile, les chaînes rangées en mémoire, les tableaux parcourus par incrément d'adresse. Comprendre les pointeurs en C, c'est comprendre d'avance une grande partie de ce qu'on verra désassemblé.

## Une variable a une adresse

Chaque variable occupe une place en mémoire, et cette place a une adresse. L'opérateur `&` donne l'adresse d'une variable :

```c
#include <stdio.h>

int main(void)
{
    int n = 42;
    printf("valeur : %d\n", n);
    printf("adresse : %p\n", (void *)&n);
    return 0;
}
```

`%p` affiche une adresse (un grand nombre en hexadécimal, par exemple `0x7ffd...`). Cette adresse est exactement ce qu'on retrouvera dans un registre comme `rbp-0x4` au chapitre 6.

## Un pointeur contient une adresse

Un **pointeur** est une variable dont la valeur est une adresse. On le déclare avec `*` :

```c
int n = 42;
int *p = &n;    // p contient l'adresse de n
```

Pour lire ou écrire la valeur pointée, on **déréférence** avec `*` :

```c
printf("%d\n", *p);   // affiche 42 (la valeur à l'adresse p)
*p = 99;              // modifie n à travers p
printf("%d\n", n);    // affiche 99
```

Résumé des deux opérateurs, à ne pas confondre :

- `&x` : l'adresse de `x` (on « prend l'adresse »).
- `*p` : la valeur à l'adresse contenue dans `p` (on « suit le pointeur »).

C'est la même idée que `[rax]` en assembleur : `rax` contient une adresse, `[rax]` est la valeur à cette adresse.

## Les chaînes de caractères

En C, une chaîne est un **tableau de caractères** terminé par un octet nul `'\0'`. Il n'y a pas de type « chaîne » : une chaîne est manipulée par un pointeur `char *` vers son premier caractère.

```c
char *s = "Clemence";
```

En mémoire : `'C'`, `'l'`, `'e'`, `'m'`, `'e'`, `'n'`, `'c'`, `'e'`, `'\0'`. Le `'\0'` final (octet de valeur 0) marque la fin : c'est lui qui permet aux fonctions de savoir où la chaîne s'arrête. Oublier ce terminateur est une source classique de bugs et de vulnérabilités, et sa recherche est un motif très reconnaissable en assembleur (chapitre 15).

On parcourt une chaîne en avançant le pointeur ou l'indice jusqu'au `'\0'` :

```c
int my_strlen(const char *s)
{
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}
```

`s[i]` est équivalent à `*(s + i)` : on ajoute `i` à l'adresse de départ, puis on déréférence. Le `const` signale que la fonction ne modifie pas la chaîne.

## Exercices

**Exercice 0.3.1** Écrire `int my_strlen(const char *s)` qui renvoie la longueur d'une chaîne, sans utiliser `strlen`. Vérifier sur `"Clemence"` (longueur 8).

**Exercice 0.3.2** Écrire `void my_putstr(const char *s)` qui affiche une chaîne caractère par caractère avec `my_putchar`, jusqu'au `'\0'`.

**Exercice 0.3.3** Écrire `char *my_revstr(char *s)` qui inverse une chaîne sur place (sans tableau intermédiaire) et renvoie `s`. Indice : échanger le premier et le dernier caractère, puis le deuxième et l'avant-dernier, etc., en avançant deux indices l'un vers l'autre. Exemple : `"assembleur"` devient `"ruelbmessa"`.

**Exercice 0.3.4** Écrire `int compter_voyelles(const char *s)` qui renvoie le nombre de voyelles (a, e, i, o, u, y, minuscules) d'une chaîne.

**Exercice 0.3.5** Écrire `void my_putstr_endl(const char *s)` qui affiche une chaîne suivie d'un retour à la ligne, en réutilisant `my_putstr`. Cela illustre la réutilisation de fonctions, exactement comme un programme appellera `my_putstr` depuis plusieurs endroits.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-03)

## Suivi

- [ ] Chapitre 0.3 : lecture
- [ ] Chapitre 0.3 : exercices
