# Chapitre 4 : Boucles

Une boucle répète un bloc d'instructions. C'est la construction la plus fréquente dans les programmes à analyser : une boucle qui transforme une chaîne, qui parcourt un tableau, qui calcule. En reverse, une boucle se repère à un saut qui revient en arrière (chapitre 4 de la partie 2). Ce chapitre approfondit ce que le chapitre 1 n'a fait qu'effleurer.

## while

Répète tant que la condition est vraie :

```c
int i = 0;
while (i < 5)
{
    my_putchar('0' + i);
    i++;              // ne pas oublier de faire avancer i, sinon boucle infinie
}
```

## for

Regroupe l'initialisation, la condition et l'incrément sur une ligne. C'est la forme la plus utilisée pour compter :

```c
for (int i = 0; i < 5; i++)
    my_putchar('0' + i);
```

Les deux boucles ci-dessus font la même chose et compilent souvent vers le même assembleur.

## do while

Comme `while`, mais le corps s'exécute **au moins une fois**, car le test est à la fin :

```c
int n;
do {
    n = lire_un_nombre();
} while (n < 0);        // recommence tant que n est négatif
```

## break et continue

- `break` sort immédiatement de la boucle.
- `continue` saute directement au tour suivant.

```c
for (int i = 0; i < 100; i++)
{
    if (i == 42) break;      // on s'arrête à 42
    if (i % 2 == 1) continue; // on ignore les impairs
    my_putnbr(i);
}
```

## Boucles imbriquées

Une boucle dans une boucle, par exemple pour un affichage en deux dimensions :

```c
for (int ligne = 1; ligne <= 4; ligne++)
{
    for (int col = 0; col < ligne; col++)
        my_putchar('*');
    my_putchar('\n');
}
```

## Afficher un nombre, sans printf

Afficher un entier avec `my_putchar` seul est un exercice clé : il faut extraire les chiffres un par un. Le principe :

- `n % 10` donne le dernier chiffre ;
- `n / 10` retire ce dernier chiffre ;
- on répète jusqu'à épuiser le nombre, en gérant le signe et le cas `0`.

Les chiffres sortent dans l'ordre inverse, donc on les stocke avant de les afficher à l'envers. Attention au plus petit entier (`-2147483648`), dont l'opposé n'est pas représentable : calculer dans un `long`. C'est l'objet de l'exercice 4.3.

## Exercices

Rappel de la méthode : un fichier `.c` par exercice, la fonction testée depuis votre propre `main`, compilation avec `gcc -Wall -Wextra`. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-04).

### Exercice 4.1 - compter (N1)

- **Fichier à rendre :** `compter.c`
- **Prototype :** `void compter(int n);`
- **Fonctions autorisées :** `printf`

Afficher les entiers de 1 à `n`, séparés par une espace.

Exemple d'exécution attendu (appel `compter(5)`) :

```text
1 2 3 4 5
```

### Exercice 4.2 - table (N1)

- **Fichier à rendre :** `table.c`
- **Prototype :** `void table(int n);`
- **Fonctions autorisées :** `printf`

Afficher la table de multiplication de `n`, de 1 à 9, une ligne par produit, sous la forme `n x i = resultat`.

Exemple d'exécution attendu (appel `table(7)`) :

```text
7 x 1 = 7
7 x 2 = 14
...
7 x 9 = 63
```

### Exercice 4.3 - my_putnbr (N2)

- **Fichier à rendre :** `my_putnbr.c`
- **Prototype :** `void my_putnbr(int n);`
- **Fonctions autorisées :** `my_putchar`

Afficher un entier (positif ou négatif) avec `my_putchar` uniquement. Gérer le signe et le cas `0`. Attention au plus petit entier (`-2147483648`), dont l'opposé n'est pas représentable : calculer dans un `long`.

Exemple d'exécution attendu (appels sur `0`, `42`, `-42`, `-2147483648`, un par ligne) :

```text
0
42
-42
-2147483648
```

### Exercice 4.4 - factorielle (N2)

- **Fichier à rendre :** `factorielle.c`
- **Prototype :** `long factorielle(int n);`
- **Fonctions autorisées :** aucune

Calculer `n!` (produit de 1 à `n`) avec une boucle.

Exemple d'exécution attendu (appel `factorielle(10)`) :

```text
3628800
```

### Exercice 4.5 - somme_chiffres (N2)

- **Fichier à rendre :** `somme_chiffres.c`
- **Prototype :** `int somme_chiffres(int n);`
- **Fonctions autorisées :** aucune

Renvoyer la somme des chiffres de `n` (en valeur absolue).

Exemple d'exécution attendu (appel `somme_chiffres(-12345)`) :

```text
15
```

### Exercice 4.6 - est_premier (N3)

- **Fichier à rendre :** `est_premier.c`
- **Prototype :** `int est_premier(int n);`
- **Fonctions autorisées :** aucune

Renvoyer 1 si `n` est premier, 0 sinon. Optimiser en ne testant les diviseurs que jusqu'à la racine de `n` (condition `d * d <= n`).

Exemple d'exécution attendu (appels sur `1`, `2`, `17`, `18`) :

```text
0 1 1 0
```

### Exercice 4.7 - triangle (N3)

- **Fichier à rendre :** `triangle.c`
- **Prototype :** `void triangle(int h);`
- **Fonctions autorisées :** `my_putchar`

Afficher un triangle de hauteur `h` fait d'étoiles (ligne 1 : une étoile, ligne 2 : deux, etc.), avec des boucles imbriquées.

Exemple d'exécution attendu (appel `triangle(4)`) :

```text
*
**
***
****
```

### Exercice 4.8 - fizzbuzz (N3)

- **Fichier à rendre :** `fizzbuzz.c`
- **Prototype :** `void fizzbuzz(void);`
- **Fonctions autorisées :** `printf`

Afficher les nombres de 1 à 100, en remplaçant les multiples de 3 par `Fizz`, les multiples de 5 par `Buzz`, et les multiples de 3 et 5 par `FizzBuzz`. C'est un grand classique d'entretien.

Exemple d'exécution attendu (début de la sortie) :

```text
1
2
Fizz
4
Buzz
Fizz
7
...
```

## Fil rouge : la boucle de vérification

On généralise la vérification d'une position (chapitre 3) à toutes les positions, avec une boucle. On utilise ici une clé candidate fixe ; la lecture de la vraie clé viendra au chapitre 9.

À vous de compléter `coffre0` :

- **Fichier à rendre :** `coffre0.c`
- **Fonctions autorisées :** `printf`

Consignes :

- Reprendre le tableau `cible`.
- Déclarer une clé candidate de 8 caractères (par exemple `'C','l','e','m','e','n','c','e'`).
- Parcourir les 8 positions : si, pour une position `i`, `(unsigned char)(cle[i] + i)` diffère de `cible[i]`, la clé est refusée.
- Afficher `Coffre ouvert` si toutes les positions correspondent, `Refuse` sinon.

Essayez de changer un caractère de la clé : le coffre refuse. C'est déjà un mini-crackme fonctionnel. Les chapitres suivants vont le rendre propre : une fonction de vérification (chapitre 5), une vraie clé lue en argument (chapitre 9), rangée dans une structure (chapitre 10).

Exemple d'exécution attendu (avec la clé ci-dessus) :

```text
Coffre ouvert
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-04)

## Suivi

- [ ] Partie 1, chapitre 4 : lecture
- [ ] Partie 1, chapitre 4 : exercices
- [ ] Partie 1, chapitre 4 : fil rouge
