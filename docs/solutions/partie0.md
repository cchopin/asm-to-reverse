# Solutions - partie 0 (les bases du C)

Chaque solution a été compilée et testée avec `gcc -Wall -Wextra`. Ouvrez-les seulement après avoir cherché par vous-même : en reverse comme ailleurs, c'est l'effort de résolution qui fait progresser.

Toutes les fonctions supposent la brique de base :

```c
#include <unistd.h>

void my_putchar(char c)
{
    write(1, &c, 1);
}
```

## Chapitre 0.1 {#chapitre-01}

**0.1.1 my_putchar** : voir ci-dessus.

**0.1.2 alphabet**

```c
void alphabet(void)
{
    for (char c = 'a'; c <= 'z'; c++)
        my_putchar(c);
    my_putchar('\n');
}
```

**0.1.3 chiffres**

```c
void chiffres(void)
{
    for (char c = '0'; c <= '9'; c++)
        my_putchar(c);
    my_putchar('\n');
}
```

La valeur ASCII de `'0'` est 48. Les dix chiffres se suivent, ce qui explique l'astuce `'0' + n` pour obtenir le caractère d'un chiffre.

**0.1.4 afficher 42**

```c
my_putchar('4');
my_putchar('2');
my_putchar('\n');
```

## Chapitre 0.2 {#chapitre-02}

**0.2.1 alphabet_inverse**

```c
void alphabet_inverse(void)
{
    for (char c = 'z'; c >= 'a'; c--)
        my_putchar(c);
    my_putchar('\n');
}
```

**0.2.2 my_putnbr**

Le piège est `-2147483648` : son opposé n'est pas représentable sur un `int`. On passe par un `long` le temps du calcul.

```c
void my_putnbr(int n)
{
    if (n < 0)
        my_putchar('-');
    long v = n < 0 ? -(long)n : n;
    if (v >= 10)
        my_putnbr((int)(v / 10));
    my_putchar('0' + (v % 10));
}
```

La récursion affiche d'abord les chiffres de poids fort : `my_putnbr(v / 10)` traite tout sauf le dernier chiffre, puis on affiche le dernier.

**0.2.3 est_pair**

```c
int est_pair(int n)
{
    return n % 2 == 0;
}
```

**0.2.4 table**

```c
void table(int n)
{
    for (int i = 1; i <= 9; i++)
    {
        my_putnbr(n);
        my_putstr(" x ");         /* ou trois my_putchar */
        my_putnbr(i);
        my_putstr(" = ");
        my_putnbr(n * i);
        my_putchar('\n');
    }
}
```

**0.2.5 puissance** (bonus)

```c
int puissance(int base, int exposant)
{
    int r = 1;
    for (int i = 0; i < exposant; i++)
        r = r * base;
    return r;
}
```

## Chapitre 0.3 {#chapitre-03}

**0.3.1 my_strlen**

```c
int my_strlen(const char *s)
{
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}
```

**0.3.2 my_putstr**

```c
void my_putstr(const char *s)
{
    int i = 0;
    while (s[i] != '\0')
        my_putchar(s[i++]);
}
```

**0.3.3 my_revstr**

Deux indices avancent l'un vers l'autre et échangent les caractères.

```c
char *my_revstr(char *s)
{
    int i = 0;
    int j = my_strlen(s) - 1;
    while (i < j)
    {
        char t = s[i];
        s[i] = s[j];
        s[j] = t;
        i++;
        j--;
    }
    return s;
}
```

`"assembleur"` devient `"ruelbmessa"`.

**0.3.4 compter_voyelles**

```c
int compter_voyelles(const char *s)
{
    int n = 0;
    for (int i = 0; s[i] != '\0'; i++)
    {
        char c = s[i];
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y')
            n++;
    }
    return n;
}
```

Pour compter aussi les majuscules, il suffit d'ajouter `A E I O U Y` au test.

**0.3.5 my_putstr_endl**

```c
void my_putstr_endl(const char *s)
{
    my_putstr(s);
    my_putchar('\n');
}
```

## Chapitre 0.4 {#chapitre-04}

**0.4.1 somme_tableau**

```c
int somme_tableau(int *tab, int taille)
{
    int s = 0;
    for (int i = 0; i < taille; i++)
        s += tab[i];
    return s;
}
```

**0.4.2 afficher argv**

```c
#include <unistd.h>

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        my_putstr_endl(argv[i]);
    return 0;
}
```

`./prog un deux trois` affiche `un`, `deux`, `trois` (on commence à `i = 1` pour sauter le nom du programme).

**0.4.3 my_getnbr**

On saute les espaces, on lit les signes (chaque `-` inverse le signe), puis on accumule les chiffres.

```c
int my_getnbr(const char *s)
{
    int i = 0;
    int signe = 1;
    long r = 0;

    while (s[i] == ' ' || s[i] == '\t')
        i++;
    while (s[i] == '+' || s[i] == '-')
    {
        if (s[i] == '-')
            signe = -signe;
        i++;
    }
    while (s[i] >= '0' && s[i] <= '9')
    {
        r = r * 10 + (s[i] - '0');
        i++;
    }
    return (int)(signe * r);
}
```

Résultats : `"42"` donne 42, `"  -42abc"` donne -42, `"+-+-13"` donne 13. Le motif `r = r * 10 + chiffre` est à retenir : on le reconnaîtra en assembleur.

**0.4.4 distance_carre**

```c
struct point
{
    int x;
    int y;
};

int distance_carre(struct point *a, struct point *b)
{
    int dx = a->x - b->x;
    int dy = a->y - b->y;
    return dx * dx + dy * dy;
}
```

Entre `{1, 2}` et `{4, 6}` : `dx = -3`, `dy = -4`, donc `9 + 16 = 25`.

**0.4.5 mini-crackme** (bonus)

```c
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc > 1 && my_getnbr(argv[1]) == 1337)
        my_putstr_endl("Gagne");
    else
        my_putstr_endl("Perdu");
    return 0;
}
```

`./prog 1337` affiche `Gagne`, tout le reste affiche `Perdu`. C'est exactement la logique qu'on désassemblera dans les premiers défis de la partie 2 : un calcul sur l'entrée, une comparaison, un saut conditionnel.
