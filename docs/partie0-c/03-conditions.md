# Chapitre 3 : Conditions

Un programme prend des décisions : faire une chose ou une autre selon une condition. En reverse, chaque condition du code source devient une comparaison suivie d'un saut (chapitre 3 de la partie 2) ; les reconnaître dans le C aide à les reconnaître une fois compilées.

## if, else if, else

```c
if (n > 0)
    printf("positif\n");
else if (n < 0)
    printf("negatif\n");
else
    printf("nul\n");
```

Le bloc exécuté est le premier dont la condition est vraie. Sans accolades, seule l'instruction qui suit le `if` est concernée ; avec accolades `{ ... }`, tout le bloc l'est. En cas de doute, mettre des accolades.

## Les comparaisons

`==` (égal), `!=` (différent), `<`, `>`, `<=`, `>=`. Elles donnent une valeur « vraie » (différente de 0) ou « fausse » (0). En C, il n'y a pas de type booléen d'origine : n'importe quelle valeur non nulle est vraie.

Piège classique à connaître : `=` est une **affectation**, `==` est un **test**. `if (n = 0)` affecte 0 à `n` (et la condition est alors fausse) ; ce n'est presque jamais ce qu'on veut.

## Combiner des conditions

- `&&` (et) : vrai si les deux côtés sont vrais.
- `||` (ou) : vrai si au moins un côté est vrai.
- `!` (non) : inverse.

```c
if (age >= 0 && age < 18)
    printf("mineur\n");
```

L'évaluation est **paresseuse** : dans `a && b`, si `a` est faux, `b` n'est pas évalué. C'est utile pour tester un pointeur avant de l'utiliser (chapitre 6).

## Le ternaire

Une forme condensée de `if/else` qui produit une valeur :

```c
int m = (a > b) ? a : b;   // m vaut le plus grand de a et b
```

À lire : « si `a > b`, alors `a`, sinon `b` ».

## switch

Quand on compare une même variable à plusieurs valeurs constantes, `switch` est plus lisible :

```c
switch (op)
{
    case '+': resultat = a + b; break;
    case '-': resultat = a - b; break;
    default:  resultat = 0;     break;
}
```

Le `break` est indispensable : sans lui, l'exécution « tombe » dans le cas suivant. On retrouvera exactement ce `switch` sous forme de table de sauts en assembleur (chapitre 7 de la partie 2).

## Exercices

Rappel de la méthode : un fichier `.c` par exercice, la fonction testée depuis votre propre `main`, compilation avec `gcc -Wall -Wextra`. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-03).

### Exercice 3.1 - signe (N1)

- **Fichier à rendre :** `signe.c`
- **Prototype :** `int signe(int n);`
- **Fonctions autorisées :** aucune

Renvoyer `1` si `n` est positif, `-1` s'il est négatif, `0` s'il est nul.

Exemple d'exécution attendu (appels `signe(-5)`, `signe(0)`, `signe(42)`) :

```text
-1 0 1
```

### Exercice 3.2 - max2 et max3 (N1)

- **Fichier à rendre :** `max.c`
- **Prototypes :** `int max2(int a, int b);` et `int max3(int a, int b, int c);`
- **Fonctions autorisées :** aucune (`max3` doit réutiliser `max2`)

Renvoyer le plus grand de deux entiers, puis le plus grand de trois en réutilisant `max2`.

Exemple d'exécution attendu (appels `max2(3, 9)`, `max3(4, 7, 2)`) :

```text
9 7
```

### Exercice 3.3 - est_bissextile (N2)

- **Fichier à rendre :** `est_bissextile.c`
- **Prototype :** `int est_bissextile(int annee);`
- **Fonctions autorisées :** aucune

Renvoyer 1 si l'année est bissextile, 0 sinon. Règle : divisible par 4, sauf les multiples de 100 qui ne sont pas multiples de 400.

Exemple d'exécution attendu (appels `est_bissextile(2000)`, `est_bissextile(1900)`, `est_bissextile(2024)`) :

```text
1 0 1
```

### Exercice 3.4 - categorie (N2)

- **Fichier à rendre :** `categorie.c`
- **Prototype :** `const char *categorie(int age);`
- **Fonctions autorisées :** aucune

Renvoyer `"enfant"` (moins de 12 ans), `"adolescent"` (moins de 18), `"adulte"` (moins de 65), `"senior"` au-delà, et `"invalide"` si l'âge est négatif.

Exemple d'exécution attendu (appels `categorie(8)`, `categorie(15)`, `categorie(40)`, `categorie(70)`, `categorie(-1)`) :

```text
enfant adolescent adulte senior invalide
```

### Exercice 3.5 - applique (N3)

- **Fichier à rendre :** `applique.c`
- **Prototype :** `int applique(int a, int b, char op);`
- **Fonctions autorisées :** aucune

Renvoyer le résultat de l'opération `op` (`'+'`, `'-'`, `'*'`, `'/'`) entre `a` et `b`, à l'aide d'un `switch`. Pour `'/'`, renvoyer 0 si `b` vaut 0 (éviter la division par zéro).

Exemple d'exécution attendu (appels `applique(6, 2, '+')`, `applique(6, 2, '/')`, `applique(6, 0, '/')`) :

```text
8 3 0
```

## Fil rouge : vérifier une position

Le coffre validera la clé position par position. La règle : pour chaque position `i`, le caractère de la clé augmenté de `i` doit être égal à `cible[i]`. On vérifie ici une seule position, la première.

À vous de compléter `coffre0` :

- **Fichier à rendre :** `coffre0.c`
- **Fonctions autorisées :** `printf`

Consignes :

- Reprendre le tableau `cible` du chapitre 2.
- Choisir un caractère candidat `c` pour la position 0.
- Afficher `position 0 correcte` si `(unsigned char)(c + 0)` est égal à `cible[0]`, sinon `position 0 incorrecte`.

Indice : `cible[0]` vaut 67, donc le bon caractère est celui tel que `c + 0 == 67`, c'est-à-dire `'C'`. Essayez d'autres caractères pour voir le message changer. Le chapitre 4 généralisera cette vérification à toutes les positions avec une boucle.

Exemple d'exécution attendu (avec `c = 'C'`) :

```text
position 0 correcte
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-03)

## Suivi

- [ ] Partie 1, chapitre 3 : lecture
- [ ] Partie 1, chapitre 3 : exercices
- [ ] Partie 1, chapitre 3 : fil rouge
