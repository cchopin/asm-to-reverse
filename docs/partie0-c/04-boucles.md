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

**Exercice 4.1 (N1)** Écrire `void compter(int n)` qui affiche les entiers de 1 à `n`, séparés par un espace (vous pouvez utiliser `printf("%d ")` ici).

**Exercice 4.2 (N1)** Écrire `void table(int n)` qui affiche la table de multiplication de `n`, de 1 à 9, une ligne par produit, sous la forme `n x i = resultat`.

**Exercice 4.3 (N2)** Écrire `void my_putnbr(int n)` qui affiche un entier (positif ou négatif) avec `my_putchar` uniquement. Le tester avec `0`, `42`, `-42` et `-2147483648`.

**Exercice 4.4 (N2)** Écrire `long factorielle(int n)` qui calcule `n!` (produit de 1 à `n`) avec une boucle. Vérifier `factorielle(10)` vaut `3628800`.

**Exercice 4.5 (N2)** Écrire `int somme_chiffres(int n)` qui renvoie la somme des chiffres de `n` (en valeur absolue). Exemple : `somme_chiffres(-12345)` vaut 15.

**Exercice 4.6 (N3)** Écrire `int est_premier(int n)` qui renvoie 1 si `n` est premier, 0 sinon. Optimiser en ne testant les diviseurs que jusqu'à la racine de `n` (condition `d * d <= n`).

**Exercice 4.7 (N3)** Écrire `void triangle(int h)` qui affiche un triangle de hauteur `h` fait d'étoiles (ligne 1 : une étoile, ligne 2 : deux, etc.), avec des boucles imbriquées.

**Exercice 4.8 (N3)** Écrire `void fizzbuzz(void)` qui affiche les nombres de 1 à 100, en remplaçant les multiples de 3 par `Fizz`, les multiples de 5 par `Buzz`, et les multiples de 3 et 5 par `FizzBuzz`. C'est un grand classique d'entretien.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-04)

## Fil rouge : la boucle de vérification

On généralise la vérification d'une position (chapitre 3) à toutes les positions, avec une boucle. On utilise ici une clé candidate fixe ; la lecture de la vraie clé viendra au chapitre 9.

```c
unsigned char cible[8] = {67, 109, 103, 112, 105, 115, 105, 108};
char cle[8] = {'C', 'l', 'e', 'm', 'e', 'n', 'c', 'e'};

int ok = 1;
for (int i = 0; i < 8; i++)
    if ((unsigned char)(cle[i] + i) != cible[i])
        ok = 0;

if (ok)
    printf("Coffre ouvert\n");
else
    printf("Refuse\n");
```

Essayez de changer un caractère de `cle` : le coffre refuse. C'est déjà un mini-crackme fonctionnel. Les chapitres suivants vont le rendre propre : une fonction de vérification (0.5), une vraie clé lue en argument (0.9), rangée dans une structure (0.10).

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-04)

## Suivi

- [ ] Partie 1, chapitre 4 : lecture
- [ ] Partie 1, chapitre 4 : exercices
- [ ] Partie 1, chapitre 4 : fil rouge
