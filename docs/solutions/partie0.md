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

**0.1.2 afficher 42**

```c
my_putchar('4');
my_putchar('2');
my_putchar('\n');
```

**0.1.3 prénom** : une suite de `my_putchar`, un par lettre, puis `my_putchar('\n')`.

**0.1.4 chiffres**

```c
void chiffres(void)
{
    for (char c = '0'; c <= '9'; c++)
        my_putchar(c);
    my_putchar('\n');
}
```

`printf("%d\n", '0');` affiche 48 : c'est le code ASCII de `'0'`, d'où l'astuce `'0' + n`.

**0.1.5 alphabet**

```c
void alphabet(void)
{
    for (char c = 'a'; c <= 'z'; c++)
        my_putchar(c);
    my_putchar('\n');
}
```

**0.1.6 alphabet_inverse et une_lettre_sur_deux**

```c
void alphabet_inverse(void)
{
    for (char c = 'z'; c >= 'a'; c--)
        my_putchar(c);
    my_putchar('\n');
}

void une_lettre_sur_deux(void)
{
    for (char c = 'a'; c <= 'z'; c += 2)
        my_putchar(c);
    my_putchar('\n');
}
```

### Fil rouge 0.1 {#fil-rouge-01}

```c
#include <stdio.h>

int main(void)
{
    printf("=== Coffre 0 ===\n");
    printf("Usage : ./coffre0 <cle>\n");
    return 0;
}
```

## Chapitre 0.2 {#chapitre-02}

**0.2.1 afficher_tailles**

```c
#include <stdio.h>

void afficher_tailles(void)
{
    printf("char = %zu\n", sizeof(char));
    printf("int = %zu\n", sizeof(int));
    printf("long = %zu\n", sizeof(long));
    printf("unsigned int = %zu\n", sizeof(unsigned int));
    printf("void * = %zu\n", sizeof(void *));
}
```

Sur une machine 64 bits classique : 1, 4, 8, 4, 8.

**0.2.2 afficher des variables**

```c
int n = -17;
char c = 'Z';
long g = 1000000000L;
printf("%d %c %ld\n", n, c, g);
```

**0.2.3 hms**

```c
void hms(int total)
{
    int h = total / 3600;
    int m = (total % 3600) / 60;
    int s = total % 60;
    printf("%dh %dm %ds\n", h, m, s);
}
```

**0.2.4 apres**

```c
unsigned char apres(unsigned char x)
{
    return x + 1;   // 255 + 1 "fait le tour" et revient a 0
}
```

**0.2.5 interpretations**

```c
void interpretations(unsigned char b)
{
    printf("%u -> %d\n", b, (signed char)b);
}
```

Résultats : 0 donne 0, 127 donne 127, 128 donne -128, 200 donne -56, 255 donne -1. Dès que le bit de poids fort est à 1 (valeur 128 ou plus), la lecture signée devient négative.

### Fil rouge 0.2 {#fil-rouge-02}

```c
#include <stdio.h>

int main(void)
{
    unsigned char cible[8] = {67, 109, 103, 112, 105, 115, 105, 108};

    for (int i = 0; i < 8; i++)
        printf("%02x ", cible[i]);
    printf("\n");
    return 0;
}
```

Affiche `43 6d 67 70 69 73 69 6c`.

## Chapitre 0.3 {#chapitre-03}

**0.3.1 signe**

```c
int signe(int n)
{
    if (n > 0) return 1;
    if (n < 0) return -1;
    return 0;
}
```

**0.3.2 max2 et max3**

```c
int max2(int a, int b)
{
    return a > b ? a : b;
}

int max3(int a, int b, int c)
{
    return max2(max2(a, b), c);
}
```

**0.3.3 est_bissextile**

```c
int est_bissextile(int annee)
{
    return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
}
```

**0.3.4 categorie**

```c
const char *categorie(int age)
{
    if (age < 0)  return "invalide";
    if (age < 12) return "enfant";
    if (age < 18) return "adolescent";
    if (age < 65) return "adulte";
    return "senior";
}
```

**0.3.5 applique**

```c
int applique(int a, int b, char op)
{
    switch (op)
    {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0;
        default:  return 0;
    }
}
```

### Fil rouge 0.3 {#fil-rouge-03}

```c
unsigned char cible[8] = {67, 109, 103, 112, 105, 115, 105, 108};
char c = 'C';

if ((unsigned char)(c + 0) == cible[0])
    printf("position 0 correcte\n");
else
    printf("position 0 incorrecte\n");
```

Le bon caractère en position 0 est `'C'` (code 67), puisque `cible[0]` vaut 67 et que l'on ajoute 0.

## Chapitre 0.4 {#chapitre-04}

**0.4.1 compter**

```c
void compter(int n)
{
    for (int i = 1; i <= n; i++)
        printf("%d ", i);
    printf("\n");
}
```

**0.4.2 table**

```c
void table(int n)
{
    for (int i = 1; i <= 9; i++)
        printf("%d x %d = %d\n", n, i, n * i);
}
```

**0.4.3 my_putnbr**

Les chiffres sortent à l'envers, on les stocke puis on les affiche en sens inverse. Le `long` évite le débordement sur `-2147483648`.

```c
void my_putnbr(int n)
{
    char buf[12];
    int i = 0;
    long v = n;

    if (v < 0) { my_putchar('-'); v = -v; }
    if (v == 0) { my_putchar('0'); return; }
    while (v > 0) { buf[i++] = '0' + (v % 10); v /= 10; }
    while (i > 0)
        my_putchar(buf[--i]);
}
```

**0.4.4 factorielle**

```c
long factorielle(int n)
{
    long r = 1;
    for (int i = 2; i <= n; i++)
        r *= i;
    return r;
}
```

**0.4.5 somme_chiffres**

```c
int somme_chiffres(int n)
{
    int s = 0;
    if (n < 0) n = -n;
    while (n > 0) { s += n % 10; n /= 10; }
    return s;
}
```

**0.4.6 est_premier**

```c
int est_premier(int n)
{
    if (n < 2) return 0;
    for (int d = 2; (long)d * d <= n; d++)
        if (n % d == 0) return 0;
    return 1;
}
```

**0.4.7 triangle**

```c
void triangle(int h)
{
    for (int i = 1; i <= h; i++)
    {
        for (int j = 0; j < i; j++)
            my_putchar('*');
        my_putchar('\n');
    }
}
```

**0.4.8 fizzbuzz**

```c
void fizzbuzz(void)
{
    for (int i = 1; i <= 100; i++)
    {
        if (i % 15 == 0)      printf("FizzBuzz");
        else if (i % 3 == 0)  printf("Fizz");
        else if (i % 5 == 0)  printf("Buzz");
        else                  printf("%d", i);
        printf("\n");
    }
}
```

### Fil rouge 0.4 {#fil-rouge-04}

```c
#include <stdio.h>

int main(void)
{
    unsigned char cible[8] = {67, 109, 103, 112, 105, 115, 105, 108};
    char cle[8] = {'C', 'l', 'e', 'm', 'e', 'n', 'c', 'e'};
    int ok = 1;

    for (int i = 0; i < 8; i++)
        if ((unsigned char)(cle[i] + i) != cible[i])
            ok = 0;

    printf("%s\n", ok ? "Coffre ouvert" : "Refuse");
    return 0;
}
```

La clé valide est `Clemence` : pour chaque position `i`, `Clemence[i] + i` reproduit exactement `cible[i]`.

## Chapitre 0.5 {#chapitre-05}

**0.5.1 my_abs**

```c
int my_abs(int n)
{
    return n < 0 ? -n : n;
}
```

**0.5.2 puissance**

```c
long puissance(int base, int exposant)
{
    long r = 1;
    for (int i = 0; i < exposant; i++)
        r *= base;
    return r;
}
```

**0.5.3 pgcd**

```c
int pgcd(int a, int b)
{
    while (b != 0)
    {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}
```

**0.5.4 somme_rec**

```c
long somme_rec(int n)
{
    if (n <= 0) return 0;
    return n + somme_rec(n - 1);
}
```

**0.5.5 fibonacci**

```c
long fibonacci(int n)                 /* recursif */
{
    if (n < 2) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

long fibonacci_iter(int n)            /* iteratif, bien plus rapide */
{
    long a = 0, b = 1;
    for (int i = 0; i < n; i++) { long s = a + b; a = b; b = s; }
    return a;
}
```

**0.5.6 rate_echange**

```c
void rate_echange(int a, int b)
{
    int t = a; a = b; b = t;   /* ne modifie que les copies locales */
}
```

Après `rate_echange(x, y)`, `x` et `y` sont inchangés : la fonction a reçu des copies. Il faut passer les adresses (chapitre 0.6).

### Fil rouge 0.5 {#fil-rouge-05}

```c
unsigned char cible[8] = {67, 109, 103, 112, 105, 115, 105, 108};

int verifier(const char *cle)
{
    for (int i = 0; i < 8; i++)
        if ((unsigned char)(cle[i] + i) != cible[i])
            return 0;
    return 1;
}
```

`verifier("Clemence")` renvoie 1.

## Chapitre 0.6 {#chapitre-06}

**0.6.1 echange**

```c
void echange(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
```

**0.6.2 incremente**

```c
void incremente(int *p)
{
    (*p)++;        /* les parenthèses sont nécessaires : *p puis ++ */
}
```

**0.6.3 div_mod**

```c
void div_mod(int a, int b, int *q, int *r)
{
    *q = a / b;
    *r = a % b;
}
```

**0.6.4 rotate3**

```c
void rotate3(int *a, int *b, int *c)
{
    int t = *a;
    *a = *c;
    *c = *b;
    *b = t;
}
```

Sur `(1, 2, 3)` : `a` reçoit 3, `b` reçoit 1, `c` reçoit 2.

**0.6.5 echange_pointeurs**

```c
void echange_pointeurs(int **a, int **b)
{
    int *t = *a;
    *a = *b;
    *b = t;
}
```

### Fil rouge 0.6 {#fil-rouge-06}

```c
int verifier(const char *cle)
{
    for (int i = 0; i < 8; i++)
        if ((unsigned char)(*(cle + i) + i) != cible[i])
            return 0;
    return 1;
}
```

`*(cle + i)` et `cle[i]` sont équivalents : base plus décalage, puis déréférencement.

## Chapitre 0.7 {#chapitre-07}

**0.7.1 my_strlen**

```c
int my_strlen(const char *s)
{
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}
```

**0.7.2 my_putstr**

```c
void my_putstr(const char *s)
{
    for (int i = 0; s[i] != '\0'; i++)
        my_putchar(s[i]);
}
```

**0.7.3 my_strcmp**

```c
int my_strcmp(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && a[i] == b[i])
        i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}
```

**0.7.4 my_strcpy**

```c
char *my_strcpy(char *dest, const char *src)
{
    int i = 0;
    while (src[i] != '\0') { dest[i] = src[i]; i++; }
    dest[i] = '\0';
    return dest;
}
```

**0.7.5 my_revstr**

```c
char *my_revstr(char *s)
{
    int i = 0, j = my_strlen(s) - 1;
    while (i < j)
    {
        char t = s[i]; s[i] = s[j]; s[j] = t;
        i++; j--;
    }
    return s;
}
```

**0.7.6 est_palindrome**

```c
int est_palindrome(const char *s)
{
    int i = 0, j = my_strlen(s) - 1;
    while (i < j)
    {
        if (s[i] != s[j]) return 0;
        i++; j--;
    }
    return 1;
}
```

**0.7.7 my_upcase**

```c
void my_upcase(char *s)
{
    for (int i = 0; s[i] != '\0'; i++)
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] -= 32;
}
```

### Fil rouge 0.7 {#fil-rouge-07}

```c
int verifier(const char *cle)
{
    if (my_strlen(cle) != 8)
        return 0;
    for (int i = 0; i < 8; i++)
        if ((unsigned char)(cle[i] + i) != cible[i])
            return 0;
    return 1;
}
```

## Chapitre 0.8 {#chapitre-08}

**0.8.1 somme_tableau**

```c
int somme_tableau(const int *tab, int n)
{
    int s = 0;
    for (int i = 0; i < n; i++)
        s += tab[i];
    return s;
}
```

**0.8.2 max_tableau**

```c
int max_tableau(const int *tab, int n)
{
    int m = tab[0];
    for (int i = 1; i < n; i++)
        if (tab[i] > m) m = tab[i];
    return m;
}
```

**0.8.3 inverser_tableau**

```c
void inverser_tableau(int *tab, int n)
{
    int i = 0, j = n - 1;
    while (i < j)
    {
        int t = tab[i]; tab[i] = tab[j]; tab[j] = t;
        i++; j--;
    }
}
```

**0.8.4 compte_occurrences**

```c
int compte_occurrences(const int *tab, int n, int x)
{
    int c = 0;
    for (int i = 0; i < n; i++)
        if (tab[i] == x) c++;
    return c;
}
```

**0.8.5 tri_bulles**

```c
void tri_bulles(int *tab, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1 - i; j++)
            if (tab[j] > tab[j + 1])
            {
                int t = tab[j]; tab[j] = tab[j + 1]; tab[j + 1] = t;
            }
}
```

**0.8.6 recherche_dicho**

```c
int recherche_dicho(const int *tab, int n, int x)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi)
    {
        int m = (lo + hi) / 2;
        if (tab[m] == x) return m;
        if (tab[m] < x) lo = m + 1;
        else hi = m - 1;
    }
    return -1;
}
```

### Fil rouge 0.8 {#fil-rouge-08}

```c
unsigned char cible[8] = {67, 109, 103, 112, 105, 115, 105, 108};

for (int i = 0; i < 8; i++)
    my_putchar((char)(cible[i] - i));
my_putchar('\n');
```

Affiche `Clemence` : on a inversé la contrainte pour fabriquer la clé, comme un keygen.

## Chapitre 0.9 {#chapitre-09}

**0.9.1 afficher les arguments**

```c
#include "libmy.h"   /* ou my_putstr defini plus haut */

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        my_putstr(argv[i]);
        my_putchar('\n');
    }
    return 0;
}
```

**0.9.2 nombre d'arguments** : afficher `argc - 1` (avec `my_putnbr` ou `printf`).

**0.9.3 my_getnbr**

```c
int my_getnbr(const char *s)
{
    int i = 0, signe = 1;
    long r = 0;

    while (s[i] == ' ' || s[i] == '\t')
        i++;
    while (s[i] == '+' || s[i] == '-')
    {
        if (s[i] == '-') signe = -signe;
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

**0.9.4 somme des arguments**

```c
int main(int argc, char **argv)
{
    int s = 0;
    for (int i = 1; i < argc; i++)
        s += my_getnbr(argv[i]);
    printf("%d\n", s);
    return 0;
}
```

**0.9.5 mini-crackme 1337**

```c
int main(int argc, char **argv)
{
    if (argc == 2 && my_getnbr(argv[1]) == 1337)
        my_putstr("Gagne\n");
    else
        my_putstr("Perdu\n");
    return 0;
}
```

### Fil rouge 0.9 {#fil-rouge-09}

```c
int main(int argc, char **argv)
{
    if (argc != 2) {
        my_putstr("Usage : ./coffre0 <cle>\n");
        return 1;
    }
    if (verifier(argv[1]))
        my_putstr("Coffre ouvert\n");
    else
        my_putstr("Refuse\n");
    return 0;
}
```

## Chapitre 0.10 {#chapitre-10}

**0.10.1 distance_carre**

```c
struct point { int x; int y; };

int distance_carre(const struct point *a, const struct point *b)
{
    int dx = a->x - b->x;
    int dy = a->y - b->y;
    return dx * dx + dy * dy;
}
```

Entre `{1,2}` et `{4,6}` : `9 + 16 = 25`.

**0.10.2 augmente**

```c
void augmente(struct point *p, int dx, int dy)
{
    p->x += dx;
    p->y += dy;
}
```

**0.10.3 moyenne**

```c
struct etudiant { const char *nom; int note; };

double moyenne(const struct etudiant *tab, int n)
{
    int s = 0;
    for (int i = 0; i < n; i++)
        s += tab[i].note;
    return (double)s / n;
}
```

**0.10.4 meilleur**

```c
const char *meilleur(const struct etudiant *tab, int n)
{
    int best = 0;
    for (int i = 1; i < n; i++)
        if (tab[i].note > tab[best].note)
            best = i;
    return tab[best].nom;
}
```

### Fil rouge 0.10 {#fil-rouge-10}

```c
struct coffre { unsigned char cible[8]; };

int verifier(const struct coffre *c, const char *cle)
{
    if (my_strlen(cle) != 8)
        return 0;
    for (int i = 0; i < 8; i++)
        if ((unsigned char)(cle[i] + i) != c->cible[i])
            return 0;
    return 1;
}
```

## Chapitre 0.11 {#chapitre-11}

**0.11.1 tableau_rempli**

```c
int *tableau_rempli(int n)
{
    int *t = malloc(n * sizeof(int));
    if (t == NULL) return NULL;
    for (int i = 0; i < n; i++)
        t[i] = i * i;
    return t;
}
```

Dans l'appelant : `int *t = tableau_rempli(5); ...; free(t);`.

**0.11.2 my_strdup**

```c
char *my_strdup(const char *s)
{
    int n = my_strlen(s);
    char *d = malloc(n + 1);
    if (d == NULL) return NULL;
    for (int i = 0; i <= n; i++)
        d[i] = s[i];
    return d;
}
```

**0.11.3 concat**

```c
char *concat(const char *a, const char *b)
{
    int na = my_strlen(a), nb = my_strlen(b);
    char *d = malloc(na + nb + 1);
    if (d == NULL) return NULL;
    int k = 0;
    for (int i = 0; i < na; i++) d[k++] = a[i];
    for (int i = 0; i < nb; i++) d[k++] = b[i];
    d[k] = '\0';
    return d;
}
```

**0.11.4 copie_agrandie**

```c
int *copie_agrandie(const int *tab, int n)
{
    int *d = malloc((n + 1) * sizeof(int));
    if (d == NULL) return NULL;
    int s = 0;
    for (int i = 0; i < n; i++) { d[i] = tab[i]; s += tab[i]; }
    d[n] = s;
    return d;
}
```

### Fil rouge 0.11 {#fil-rouge-11}

```c
char *copie = my_strdup(argv[1]);
if (copie == NULL)
    return 1;
int ouvert = verifier(&coffre, copie);
free(copie);
my_putstr(ouvert ? "Coffre ouvert\n" : "Refuse\n");
```

## Chapitre 0.12 {#chapitre-12}

**0.12.1 à 0.12.3** : voir le projet complet ci-dessous. La répartition des fonctions dans `libmy.c`/`libmy.h`, le `main.c` de test et le `Makefile` (avec `clean`) y figurent. `make` ne recompile un `.o` que si son `.c` ou un `.h` dont il dépend a changé : modifier `main.c` ne recompile pas `libmy.o`.

### Fil rouge 0.12 {#fil-rouge-12}

Le coffre assemblé en projet (`libmy` + `coffre0.c` + `Makefile`) est donné en entier au chapitre [0.13](../partie0-c/13-projet.md) et reproduit ci-dessous.

## Projet : ma_libc {#projet-malibc}

`ma_libc.h`

```c
#ifndef MA_LIBC_H
#define MA_LIBC_H

int   my_strlen(const char *s);
int   my_strcmp(const char *a, const char *b);
char *my_strdup(const char *s);
char *my_strcat(char *dest, const char *src);

#endif
```

`ma_libc.c`

```c
#include <stdlib.h>
#include "ma_libc.h"

int my_strlen(const char *s)
{
    int i = 0;
    while (s[i]) i++;
    return i;
}

int my_strcmp(const char *a, const char *b)
{
    int i = 0;
    while (a[i] && a[i] == b[i]) i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}

char *my_strdup(const char *s)
{
    int n = my_strlen(s);
    char *d = malloc(n + 1);
    if (d == NULL) return NULL;
    for (int i = 0; i <= n; i++) d[i] = s[i];
    return d;
}

char *my_strcat(char *dest, const char *src)
{
    int i = my_strlen(dest), j = 0;
    while (src[j]) dest[i++] = src[j++];
    dest[i] = '\0';
    return dest;
}
```

Un `main.c` de test et un `Makefile` sur le modèle de celui du coffre complètent le projet. `my_strcat("Cle" dans un tampon assez grand, "mence")` donne `"Clemence"`.

## Projet : le coffre {#projet-coffre}

Le code complet (`libmy.h`, `libmy.c`, `coffre0.c`, `Makefile`) est donné au chapitre [0.13](../partie0-c/13-projet.md). `./coffre0 Clemence` affiche `Coffre ouvert`, tout le reste `Refuse`.
