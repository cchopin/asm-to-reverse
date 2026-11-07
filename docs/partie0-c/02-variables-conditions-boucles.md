# 0.2 Variables, conditions et boucles

On revient ici sur les briques de tout programme : les variables, les tests et les boucles. En reverse, chacune de ces constructions laisse une trace reconnaissable en assembleur (chapitres 3 et 4). Les reconnaître dans le code source aide à les reconnaître une fois compilées.

## Les types de base

Quelques types suffisent pour commencer :

```c
int   n = 42;        // entier signé, 4 octets le plus souvent
char  c = 'A';       // un octet, souvent un caractère
long  g = 123456789; // entier plus large, 8 octets
unsigned int u = 7;  // entier non signé
```

La taille et le caractère signé/non signé comptent beaucoup en reverse : ils expliquent par exemple pourquoi un même octet peut valoir `200` ou `-56` (c'est la représentation des nombres négatifs, détaillée plus tard au chapitre 1).

## Les conditions

```c
if (n > 0)
    my_putchar('+');
else if (n < 0)
    my_putchar('-');
else
    my_putchar('0');
```

Les comparaisons : `==` (égal), `!=` (différent), `<`, `>`, `<=`, `>=`. Les combinaisons : `&&` (et), `||` (ou), `!` (non).

Attention au piège classique : `=` est une affectation, `==` est un test. `if (n = 0)` affecte `0` à `n` et ne teste rien.

## Les boucles

La boucle `while` répète tant qu'une condition est vraie :

```c
int i = 0;
while (i < 10)
{
    my_putchar('0' + i);
    i = i + 1;
}
```

La boucle `for` regroupe initialisation, condition et incrément :

```c
for (int i = 0; i < 10; i++)
    my_putchar('0' + i);
```

`i++` est un raccourci pour `i = i + 1`. Les deux boucles compilent souvent vers le même assembleur (un test et un saut conditionnel, chapitre 4).

## Afficher un nombre

Afficher un entier sans `printf` demande un peu de travail : il faut en extraire les chiffres un par un. C'est exactement ce que fait la libc en interne, et le voir une fois aide à lire ce genre de routine en assembleur. C'est l'objet de l'exercice 0.2.2 ci-dessous.

Le principe :

- `n % 10` (le reste de la division par 10) donne le **dernier chiffre** ;
- `n / 10` (la division entière) **retire** ce dernier chiffre ;
- pour afficher dans le bon ordre (de gauche à droite), on affiche d'abord tout ce qui précède le dernier chiffre, puis le dernier : c'est naturel par **récursion** (une fonction qui s'appelle elle-même) ou avec une boucle.

Deux pièges à prévoir :

- un nombre négatif : afficher le signe `-`, puis travailler sur sa valeur absolue, et veiller à ce que `% 10` porte sur un nombre positif ;
- le plus petit entier, `-2147483648` : son opposé `-n` n'est pas représentable sur un `int`. Un moyen simple de l'éviter est de calculer dans un `long`. La solution détaillée montre comment.

## Exercices

**Exercice 0.2.1** Écrire `void alphabet_inverse(void)` qui affiche `zyxwvutsrqponmlkjihgfedcba` suivi d'un retour à la ligne.

**Exercice 0.2.2** Écrire `void my_putnbr(int n)` qui affiche un entier (positif ou négatif) avec `my_putchar` uniquement. Le tester avec `0`, `42`, `-42`, et `-2147483648`.

**Exercice 0.2.3** Écrire `int est_pair(int n)` qui renvoie `1` si `n` est pair, `0` sinon. Rappel : un nombre est pair si `n % 2 == 0`.

**Exercice 0.2.4** Écrire `void table(int n)` qui affiche la table de multiplication de `n` de 1 à 9, une ligne par produit, sous la forme `n x i = resultat`. Utiliser `my_putnbr` et `my_putchar`.

**Exercice 0.2.5** (bonus) Écrire `int puissance(int base, int exposant)` qui calcule `base` élevé à la puissance `exposant` (exposant positif ou nul), à l'aide d'une boucle.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-02)

## Suivi

- [ ] Chapitre 0.2 : lecture
- [ ] Chapitre 0.2 : exercices
