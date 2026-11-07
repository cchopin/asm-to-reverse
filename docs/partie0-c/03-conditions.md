# 0.3 Conditions

Un programme prend des décisions : faire une chose ou une autre selon une condition. En reverse, chaque condition du code source devient une comparaison suivie d'un saut (chapitre 3) ; les reconnaître dans le C aide à les reconnaître une fois compilées.

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

L'évaluation est **paresseuse** : dans `a && b`, si `a` est faux, `b` n'est pas évalué. C'est utile pour tester un pointeur avant de l'utiliser (chapitre 0.6).

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

Le `break` est indispensable : sans lui, l'exécution « tombe » dans le cas suivant. On retrouvera exactement ce `switch` sous forme de table de sauts en assembleur (chapitre 7).

## Exercices

**Exercice 0.3.1 (N1)** Écrire `int signe(int n)` qui renvoie `1` si `n` est positif, `-1` s'il est négatif, `0` s'il est nul.

**Exercice 0.3.2 (N1)** Écrire `int max2(int a, int b)` qui renvoie le plus grand des deux, puis `int max3(int a, int b, int c)` qui réutilise `max2`.

**Exercice 0.3.3 (N2)** Écrire `int est_bissextile(int annee)` qui renvoie 1 si l'année est bissextile, 0 sinon. Règle : divisible par 4, sauf les multiples de 100 qui ne sont pas multiples de 400. Tester 2000 (oui), 1900 (non), 2024 (oui).

**Exercice 0.3.4 (N2)** Écrire `const char *categorie(int age)` qui renvoie `"enfant"` (moins de 12), `"adolescent"` (moins de 18), `"adulte"` (moins de 65), `"senior"` au-delà, et `"invalide"` si l'âge est négatif.

**Exercice 0.3.5 (N3)** Écrire `int applique(int a, int b, char op)` qui renvoie le résultat de l'opération `op` (`'+'`, `'-'`, `'*'`, `'/'`) entre `a` et `b`, avec un `switch`. Pour `'/'`, renvoyer 0 si `b` vaut 0 (éviter la division par zéro).

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-03)

## Fil rouge : vérifier une position

Le coffre validera la clé position par position. La règle est la suivante : pour chaque position `i`, le caractère de la clé plus `i` doit être égal à `cible[i]`.

On vérifie ici une seule position, la première. `cible[0]` vaut 67, donc le bon caractère est celui tel que `c + 0 == 67`, c'est-à-dire `'C'`.

```c
unsigned char cible[8] = {67, 109, 103, 112, 105, 115, 105, 108};
char c = 'C';   // caractère candidat pour la position 0

if ((unsigned char)(c + 0) == cible[0])
    printf("position 0 correcte\n");
else
    printf("position 0 incorrecte\n");
```

Essayez avec d'autres caractères pour voir le message changer. Le chapitre 0.4 généralisera cette vérification à toutes les positions avec une boucle.

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-03)

## Suivi

- [ ] Chapitre 0.3 : lecture
- [ ] Chapitre 0.3 : exercices
- [ ] Chapitre 0.3 : fil rouge
