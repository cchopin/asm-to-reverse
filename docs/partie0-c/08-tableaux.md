# Chapitre 8 : Tableaux

Un tableau est une suite de cases du même type, rangées côte à côte en mémoire. Les chaînes du chapitre précédent en sont un cas particulier. En reverse, un tableau se repère à un accès régulier à des adresses qui avancent d'un pas fixe (chapitre 6 de la partie 2) ; les algorithmes de ce chapitre (parcours, tri, recherche) sont parmi les plus fréquents à reconnaître.

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

Un fichier `.c` par exercice, testé depuis votre propre `main`, compilé avec `gcc -Wall -Wextra`. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-08).

!!! warning "Contraintes de la série"
    Chaque fonction reçoit le tableau et sa taille. Les « fonctions autorisées » excluent toute fonction de bibliothèque de tri ou de recherche : on écrit les algorithmes à la main.

### Exercice 8.1 - somme_tableau (N1)

- **Fichier à rendre :** `somme_tableau.c`
- **Prototype :** `int somme_tableau(const int *tab, int n);`
- **Fonctions autorisées :** aucune

Renvoyer la somme des `n` éléments.

Exemple d'exécution attendu (tableau `{10, 20, 30, 40, 50}`, `n = 5`) :

```text
150
```

### Exercice 8.2 - max_tableau (N1)

- **Fichier à rendre :** `max_tableau.c`
- **Prototype :** `int max_tableau(const int *tab, int n);`
- **Fonctions autorisées :** aucune

Renvoyer le plus grand élément (on suppose `n >= 1`).

Exemple d'exécution attendu (tableau `{3, 41, 7, 18}`, `n = 4`) :

```text
41
```

### Exercice 8.3 - inverser_tableau (N2)

- **Fichier à rendre :** `inverser_tableau.c`
- **Prototype :** `void inverser_tableau(int *tab, int n);`
- **Fonctions autorisées :** aucune

Inverser l'ordre des éléments sur place.

Exemple d'exécution attendu (tableau `{1, 2, 3, 4, 5}` avant l'appel) :

```text
5 4 3 2 1
```

### Exercice 8.4 - compte_occurrences (N2)

- **Fichier à rendre :** `compte_occurrences.c`
- **Prototype :** `int compte_occurrences(const int *tab, int n, int x);`
- **Fonctions autorisées :** aucune

Renvoyer le nombre de fois où `x` apparaît.

Exemple d'exécution attendu (tableau `{1, 3, 3, 7, 3}`, `n = 5`, `x = 3`) :

```text
3
```

### Exercice 8.5 - tri_bulles (N3)

- **Fichier à rendre :** `tri_bulles.c`
- **Prototype :** `void tri_bulles(int *tab, int n);`
- **Fonctions autorisées :** aucune

Trier le tableau en ordre croissant par la méthode du tri à bulles (échanger deux voisins mal ordonnés, en répétant jusqu'à ce que plus rien ne bouge).

Exemple d'exécution attendu (tableau `{5, 2, 8, 1, 9, 3}` avant l'appel) :

```text
1 2 3 5 8 9
```

### Exercice 8.6 - recherche_dicho (N3)

- **Fichier à rendre :** `recherche_dicho.c`
- **Prototype :** `int recherche_dicho(const int *tab, int n, int x);`
- **Fonctions autorisées :** aucune

Sur un tableau **déjà trié**, renvoyer l'indice de `x` par recherche dichotomique (couper l'intervalle en deux à chaque étape), ou `-1` si absent.

Exemple d'exécution attendu (tableau trié `{1, 4, 7, 9, 12, 20}`, appels sur `9` puis `5`) :

```text
3 -1
```

## Fil rouge : retrouver la clé à partir de la cible

Jusqu'ici on connaît la clé et on vérifie. Un reverser fait l'inverse : il a la cible (les huit octets du binaire) et doit en déduire la clé. Comme la règle est `cle[i] + i == cible[i]`, il suffit de calculer `cible[i] - i` pour chaque position. C'est exactement la démarche que vous appliquerez en partie 3 : lire la contrainte dans le code désassemblé, puis l'inverser pour fabriquer la clé (un keygen).

À vous d'écrire ce petit programme :

- **Fichier à rendre :** `keygen0.c`
- **Fonctions autorisées :** `my_putchar`

Consignes :

- Reprendre le tableau `cible`.
- Pour chaque position `i`, afficher le caractère `(char)(cible[i] - i)`, puis un retour à la ligne final.

Exemple d'exécution attendu :

```text
Clemence
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-08)

## Suivi

- [ ] Partie 1, chapitre 8 : lecture
- [ ] Partie 1, chapitre 8 : exercices
- [ ] Partie 1, chapitre 8 : fil rouge
