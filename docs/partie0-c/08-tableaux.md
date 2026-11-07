# Chapitre 8 : Tableaux

Un tableau est une suite de cases du même type, rangées côte à côte en mémoire. Les chaînes du chapitre précédent en sont un cas particulier. En reverse, un tableau se repère à un accès régulier à des adresses qui avancent d'un pas fixe (chapitre 6) ; les algorithmes de ce chapitre (parcours, tri, recherche) sont parmi les plus fréquents à reconnaître.

## Déclarer et accéder

```c
int tab[5] = {10, 20, 30, 40, 50};
printf("%d\n", tab[0]);   // 10 (les indices commencent à 0)
printf("%d\n", tab[4]);   // 50
```

Il n'y a **aucun contrôle des bornes** en C : lire ou écrire `tab[5]` ou au-delà ne provoque pas d'erreur immédiate mais touche de la mémoire quelconque. C'est la base d'une grande partie des vulnérabilités (chapitre 3 de la partie 4).

## Tableau et pointeur

Le nom d'un tableau se comporte presque comme un pointeur vers sa première case : `tab[i]` est `*(tab + i)`. C'est pourquoi une fonction qui reçoit un tableau le reçoit en réalité comme un pointeur, accompagné de sa taille (le tableau seul ne connaît pas sa longueur) :

```c
int somme(const int *tab, int n)   // int tab[] signifierait la même chose
{
    int s = 0;
    for (int i = 0; i < n; i++)
        s += tab[i];
    return s;
}
```

## Exercices

!!! warning "Contraintes"
    Chaque fonction reçoit le tableau et sa taille. Interdiction d'utiliser des fonctions de bibliothèque de tri ou de recherche : on écrit les algorithmes à la main.

**Exercice 8.1 (N1)** Écrire `int somme_tableau(const int *tab, int n)` qui renvoie la somme des `n` éléments.

**Exercice 8.2 (N1)** Écrire `int max_tableau(const int *tab, int n)` qui renvoie le plus grand élément (on suppose `n >= 1`).

**Exercice 8.3 (N2)** Écrire `void inverser_tableau(int *tab, int n)` qui inverse l'ordre des éléments sur place.

**Exercice 8.4 (N2)** Écrire `int compte_occurrences(const int *tab, int n, int x)` qui renvoie le nombre de fois où `x` apparaît.

**Exercice 8.5 (N3)** Écrire `void tri_bulles(int *tab, int n)` qui trie le tableau en ordre croissant par la méthode du tri à bulles (échanger deux voisins mal ordonnés, en répétant jusqu'à ce que plus rien ne bouge).

**Exercice 8.6 (N3)** Écrire `int recherche_dicho(const int *tab, int n, int x)` qui, sur un tableau **déjà trié**, renvoie l'indice de `x` par recherche dichotomique (couper l'intervalle en deux à chaque étape), ou `-1` si absent.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-08)

## Fil rouge : retrouver la clé à partir de la cible

Jusqu'ici on connaît la clé et on vérifie. Un reverser fait l'inverse : il a la cible (les huit octets du binaire) et doit en déduire la clé. Comme la règle est `cle[i] + i == cible[i]`, il suffit de calculer `cible[i] - i` pour chaque position.

```c
unsigned char cible[8] = {67, 109, 103, 112, 105, 115, 105, 108};

for (int i = 0; i < 8; i++)
    my_putchar((char)(cible[i] - i));
my_putchar('\n');
```

Ce petit programme affiche `Clemence`. C'est exactement la démarche que vous appliquerez en partie 3 : lire la contrainte dans le code désassemblé, puis l'inverser pour fabriquer la clé (un keygen).

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-08)

## Suivi

- [ ] Partie 1, chapitre 8 : lecture
- [ ] Partie 1, chapitre 8 : exercices
- [ ] Partie 1, chapitre 8 : fil rouge
