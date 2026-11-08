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

**Exercice 5.1 (N1)** Écrire `int my_abs(int n)` qui renvoie la valeur absolue de `n`.

**Exercice 5.2 (N1)** Écrire `long puissance(int base, int exposant)` (exposant positif ou nul) avec une boucle. Vérifier `puissance(2, 10)` vaut 1024.

**Exercice 5.3 (N2)** Écrire `int pgcd(int a, int b)` par l'algorithme d'Euclide : tant que `b` n'est pas nul, remplacer le couple `(a, b)` par `(b, a % b)`. Vérifier `pgcd(48, 36)` vaut 12.

**Exercice 5.4 (N2)** Écrire `long somme_rec(int n)` qui calcule la somme de 1 à `n` de façon récursive. Vérifier `somme_rec(100)` vaut 5050.

**Exercice 5.5 (N3)** Écrire `long fibonacci(int n)` de façon récursive (`fib(0) = 0`, `fib(1) = 1`, `fib(n) = fib(n-1) + fib(n-2)`). Vérifier `fibonacci(10)` vaut 55. En bonus, en écrire une version avec une boucle, bien plus rapide.

**Exercice 5.6 (N3)** Écrire `void rate_echange(int a, int b)` qui tente d'échanger `a` et `b`, puis constater dans un `main` que `x` et `y` ne changent pas. Expliquer pourquoi. Ce constat motive le chapitre suivant.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-05)

## Fil rouge : une fonction de vérification

On transforme la boucle du chapitre 4 en une fonction propre, qui reçoit la clé et renvoie 1 si elle ouvre le coffre, 0 sinon. La clé est reçue par une adresse (`const char *`) ; on l'utilise comme un tableau de caractères, le chapitre 6 détaillera pourquoi.

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

On peut la tester avec `verifier("Clemence")` (renvoie 1). Le chapitre 7 ajoutera la vérification de la longueur.

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-05)

## Suivi

- [ ] Partie 1, chapitre 5 : lecture
- [ ] Partie 1, chapitre 5 : exercices
- [ ] Partie 1, chapitre 5 : fil rouge
