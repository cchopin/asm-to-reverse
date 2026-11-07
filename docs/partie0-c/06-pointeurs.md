# 0.6 Pointeurs

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

`%p` affiche une adresse (un grand nombre en hexadécimal, par exemple `0x7ffd...`). Le `(void *)` devant `&n` est une conversion : `%p` attend une adresse « sans type précis ». Cette adresse d'une variable locale est exactement ce qu'on manipulera en assembleur, où elle s'écrit `[rbp-0x4]` (chapitre 6).

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

Les deux opérateurs, à ne pas confondre :

- `&x` : l'adresse de `x` (on « prend l'adresse »).
- `*p` : la valeur à l'adresse contenue dans `p` (on « suit le pointeur »).

C'est la même idée que `[rax]` en assembleur : `rax` contient une adresse, `[rax]` est la valeur à cette adresse.

## Passage par adresse

Le chapitre 0.5 a montré qu'une fonction ne peut pas modifier les variables de l'appelant par passage par valeur. En passant une **adresse**, elle le peut :

```c
void echange(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int main(void)
{
    int x = 1, y = 2;
    echange(&x, &y);   // on passe les adresses
    // x vaut 2, y vaut 1
}
```

C'est aussi ainsi qu'une fonction renvoie plusieurs résultats : on lui passe des adresses où écrire.

## Le pointeur nul

Un pointeur peut ne désigner aucune adresse valide : il vaut alors `NULL`. Déréférencer `NULL` plante le programme (le fameux « segmentation fault »). On teste donc souvent `if (p != NULL)` avant d'utiliser `p`.

## Pointeur de pointeur

Puisqu'un pointeur est une variable, il a lui-même une adresse, que l'on peut stocker dans un pointeur de pointeur `int **`. On en a besoin dès qu'une fonction doit modifier un pointeur de l'appelant, et c'est la forme exacte de `argv` (chapitre 0.9).

```c
void echange_pointeurs(int **a, int **b)
{
    int *t = *a;
    *a = *b;
    *b = t;
}
```

## Exercices

**Exercice 0.6.1 (N1)** Écrire `void echange(int *a, int *b)` qui échange les valeurs pointées. Le tester et vérifier que les variables de l'appelant changent bien.

**Exercice 0.6.2 (N1)** Écrire `void incremente(int *p)` qui ajoute 1 à la valeur pointée.

**Exercice 0.6.3 (N2)** Écrire `void div_mod(int a, int b, int *q, int *r)` qui range le quotient de `a / b` dans `*q` et le reste dans `*r`. C'est l'exemple type d'une fonction à deux résultats.

**Exercice 0.6.4 (N2)** Écrire `void rotate3(int *a, int *b, int *c)` qui décale les trois valeurs vers la droite : l'ancien `c` va dans `a`, l'ancien `a` dans `b`, l'ancien `b` dans `c`. Sur `(1, 2, 3)` on obtient `(3, 1, 2)`.

**Exercice 0.6.5 (N3)** Écrire `void echange_pointeurs(int **a, int **b)` qui échange deux pointeurs eux-mêmes (pas les valeurs pointées). Vérifier qu'après l'appel, le premier pointeur désigne ce que désignait le second.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-06)

## Fil rouge : lire la clé par pointeur

La fonction `verifier` reçoit la clé par une adresse. On peut accéder à ses caractères de deux façons équivalentes : `cle[i]` ou `*(cle + i)`. La seconde montre bien ce qui se passe : on part de l'adresse de la clé, on avance de `i`, on déréférence.

```c
int verifier(const char *cle)
{
    for (int i = 0; i < 8; i++)
        if ((unsigned char)(*(cle + i) + i) != cible[i])
            return 0;
    return 1;
}
```

C'est exactement ce calcul d'adresse (base plus décalage) que l'on retrouvera en assembleur au chapitre 6.

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-06)

## Suivi

- [ ] Chapitre 0.6 : lecture
- [ ] Chapitre 0.6 : exercices
- [ ] Chapitre 0.6 : fil rouge
