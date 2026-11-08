# Chapitre 5 : Fonctions

Une fonction est un morceau de programme nommé, qui prend des arguments et renvoie éventuellement une valeur. Découper en fonctions rend le code lisible et réutilisable. En reverse, chaque fonction du source devient un bloc avec un prologue et un épilogue reconnaissables (chapitre 5 de la partie 2) : savoir où commence et finit une fonction est une compétence de base.

## Définir et appeler

```c
int carre(int x)       // type de retour, nom, paramètres
{
    return x * x;      // renvoie une valeur
}

int main(void)
{
    int y = carre(5);  // appel : y vaut 25
    return 0;
}
```

Une fonction qui ne renvoie rien a le type de retour `void`. Une fonction doit être définie (ou déclarée) **avant** d'être appelée.

## Prototypes

Pour appeler une fonction définie plus bas dans le fichier, on la **déclare** en haut par un prototype (sa signature suivie d'un point-virgule) :

```c
int carre(int x);      // prototype : promesse que la fonction existe

int main(void)
{
    return carre(5);
}

int carre(int x)       // définition, plus bas
{
    return x * x;
}
```

Les prototypes sont au cœur des fichiers d'en-tête `.h` (chapitre 12).

## Portée des variables

Une variable déclarée dans une fonction (ou un bloc) n'existe que là : c'est sa **portée**. Deux fonctions peuvent utiliser le même nom `i` sans se gêner. Une variable locale naît à l'entrée de la fonction et disparaît à sa sortie ; en mémoire, elle vit sur la pile (chapitre 5 de la partie 2).

## Passage par valeur

En C, les arguments sont passés **par valeur** : la fonction reçoit une copie. Modifier un paramètre ne change pas la variable d'origine.

```c
void rate_echange(int a, int b)   // ne marche PAS
{
    int t = a; a = b; b = t;      // ne modifie que les copies locales
}
```

Après `rate_echange(x, y)`, `x` et `y` sont inchangés. Pour modifier les variables de l'appelant, il faut passer leur **adresse** : c'est tout l'objet du chapitre 6.

## Récursion

Une fonction peut s'appeler elle-même. Il faut toujours un cas d'arrêt, sinon la récursion est infinie.

```c
long factorielle(int n)
{
    if (n <= 1) return 1;          // cas d'arrêt
    return (long)n * factorielle(n - 1);
}
```

Chaque appel empile ses propres variables locales ; on verra cet empilement concrètement en GDB (chapitre 3 de la partie 3).

## Exercices

Rappel de la méthode : un fichier `.c` par exercice, la fonction testée depuis votre propre `main`, compilation avec `gcc -Wall -Wextra`. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-05).

### Exercice 5.1 - my_abs (N1)

- **Fichier à rendre :** `my_abs.c`
- **Prototype :** `int my_abs(int n);`
- **Fonctions autorisées :** aucune

Renvoyer la valeur absolue de `n`.

Exemple d'exécution attendu (appels `my_abs(-7)`, `my_abs(7)`) :

```text
7 7
```

### Exercice 5.2 - puissance (N1)

- **Fichier à rendre :** `puissance.c`
- **Prototype :** `long puissance(int base, int exposant);`
- **Fonctions autorisées :** aucune

Calculer `base` élevé à la puissance `exposant` (exposant positif ou nul) avec une boucle.

Exemple d'exécution attendu (appel `puissance(2, 10)`) :

```text
1024
```

### Exercice 5.3 - pgcd (N2)

- **Fichier à rendre :** `pgcd.c`
- **Prototype :** `int pgcd(int a, int b);`
- **Fonctions autorisées :** aucune

Calculer le plus grand commun diviseur par l'algorithme d'Euclide : tant que `b` n'est pas nul, remplacer le couple `(a, b)` par `(b, a % b)`.

Exemple d'exécution attendu (appel `pgcd(48, 36)`) :

```text
12
```

### Exercice 5.4 - somme_rec (N2)

- **Fichier à rendre :** `somme_rec.c`
- **Prototype :** `long somme_rec(int n);`
- **Fonctions autorisées :** aucune (fonction récursive)

Calculer la somme de 1 à `n` de façon récursive (un cas d'arrêt, puis un appel à soi-même).

Exemple d'exécution attendu (appel `somme_rec(100)`) :

```text
5050
```

### Exercice 5.5 - fibonacci (N3)

- **Fichier à rendre :** `fibonacci.c`
- **Prototype :** `long fibonacci(int n);`
- **Fonctions autorisées :** aucune

Écrire la version récursive (`fib(0) = 0`, `fib(1) = 1`, `fib(n) = fib(n-1) + fib(n-2)`). En bonus, écrire une version avec une boucle, bien plus rapide.

Exemple d'exécution attendu (appel `fibonacci(10)`) :

```text
55
```

### Exercice 5.6 - rate_echange (N3)

- **Fichier à rendre :** `rate_echange.c`
- **Prototype :** `void rate_echange(int a, int b);`
- **Fonctions autorisées :** `printf` (pour le constat dans le `main`)

Tenter d'échanger `a` et `b`, puis constater dans un `main` que les variables `x` et `y` passées à l'appel ne changent pas. Expliquer pourquoi en commentaire (le passage par valeur). Ce constat motive le chapitre suivant.

Exemple d'exécution attendu (avec `x = 3`, `y = 8`) :

```text
avant : 3 8
apres : 3 8
```

## Fil rouge : une fonction de vérification

On transforme la boucle du chapitre 4 en une fonction propre, qui reçoit la clé et renvoie 1 si elle ouvre le coffre, 0 sinon.

À vous de compléter `coffre0` :

- **Fichier à rendre :** `coffre0.c`
- **Prototype :** `int verifier(const char *cle);`
- **Fonctions autorisées :** aucune

Consignes :

- Garder le tableau `cible` accessible à la fonction.
- Écrire `verifier`, qui parcourt les 8 positions et renvoie 0 dès qu'une position ne correspond pas, 1 si toutes correspondent.
- La clé est reçue par une adresse (`const char *`), que l'on utilise comme un tableau de caractères ; le chapitre 6 détaillera pourquoi.

Vous pouvez la tester avec `verifier("Clemence")` (renvoie 1). Le chapitre 7 ajoutera la vérification de la longueur.

Exemple d'exécution attendu (affichage de `verifier("Clemence")` puis `verifier("mauvais")`) :

```text
1 0
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-05)

## Suivi

- [ ] Partie 1, chapitre 5 : lecture
- [ ] Partie 1, chapitre 5 : exercices
- [ ] Partie 1, chapitre 5 : fil rouge
