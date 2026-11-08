# Chapitre 10 : Structures

Une structure regroupe plusieurs valeurs, de types éventuellement différents, sous un seul nom. C'est ainsi qu'un programme range un objet : un point, un étudiant, un joueur. En reverse, une structure se repère aux accès réguliers à des décalages fixes depuis une même adresse de base (`[rdi]`, `[rdi+4]`, chapitre 6 de la partie 2) ; savoir les reconnaître aide à reconstruire les données manipulées.

## Déclarer et utiliser

```c
struct point
{
    int x;
    int y;
};

struct point a = {1, 2};
printf("%d %d\n", a.x, a.y);   // accès avec un point
```

Les champs sont rangés les uns après les autres en mémoire, dans l'ordre de déclaration (à d'éventuels octets de remplissage près, pour l'alignement).

## Structure et pointeur

Quand on a un pointeur vers une structure, on accède aux champs avec la flèche `->` :

```c
void augmente(struct point *p, int dx, int dy)
{
    p->x += dx;        // p->x est un raccourci pour (*p).x
    p->y += dy;
}
```

Passer une structure par pointeur évite d'en copier tout le contenu, et permet de la modifier. C'est la forme qu'on rencontre presque toujours dans le code compilé.

## Tableau de structures

```c
struct etudiant
{
    const char *nom;
    int note;
};

struct etudiant classe[3] = {{"a", 12}, {"b", 15}, {"c", 18}};
```

On le parcourt comme tout tableau : `classe[i].note`.

## Exercices

Un fichier `.c` par exercice, testé depuis votre propre `main`, compilé avec `gcc -Wall -Wextra`. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-10).

### Exercice 10.1 - distance_carre (N1)

- **Fichier à rendre :** `distance_carre.c`
- **Type à définir :** `struct point { int x; int y; };`
- **Prototype :** `int distance_carre(const struct point *a, const struct point *b);`
- **Fonctions autorisées :** aucune

Renvoyer `(ax-bx)^2 + (ay-by)^2` (on reste en entiers, sans racine).

Exemple d'exécution attendu (points `{1, 2}` et `{4, 6}`) :

```text
25
```

### Exercice 10.2 - augmente (N2)

- **Fichier à rendre :** `augmente.c`
- **Prototype :** `void augmente(struct point *p, int dx, int dy);`
- **Fonctions autorisées :** `printf` (pour le test)

Déplacer un point en modifiant la structure via le pointeur. Vérifier l'effet dans un `main`.

Exemple d'exécution attendu (point `{1, 2}`, appel `augmente(&p, 3, 4)`) :

```text
4 6
```

### Exercice 10.3 - moyenne (N2)

- **Fichier à rendre :** `moyenne.c`
- **Type à définir :** `struct etudiant { const char *nom; int note; };`
- **Prototype :** `double moyenne(const struct etudiant *tab, int n);`
- **Fonctions autorisées :** `printf` (pour le test)

Renvoyer la moyenne des notes.

Exemple d'exécution attendu (notes `12`, `15`, `18`) :

```text
15.00
```

### Exercice 10.4 - meilleur (N3)

- **Fichier à rendre :** `meilleur.c`
- **Prototype :** `const char *meilleur(const struct etudiant *tab, int n);`
- **Fonctions autorisées :** aucune

Renvoyer le nom de l'étudiant ayant la meilleure note.

Exemple d'exécution attendu (étudiants `{"Ada", 12}`, `{"Bo", 18}`, `{"Cy", 15}`) :

```text
Bo
```

## Fil rouge : ranger le coffre dans une structure

On regroupe les données du coffre (sa cible) dans une structure, et `verifier` reçoit un pointeur vers ce coffre. C'est plus propre, et surtout c'est la forme qu'on retrouvera désassemblée : un pointeur vers une zone de données, lu à décalages fixes.

À vous de compléter `coffre0` :

- **Fichier à rendre :** `coffre0.c`
- **Type à définir :** `struct coffre { unsigned char cible[8]; };`
- **Prototype :** `int verifier(const struct coffre *c, const char *cle);`
- **Fonctions autorisées :** `my_strlen`

Consignes :

- Définir la structure `coffre` qui contient le tableau `cible`.
- Adapter `verifier` pour qu'elle reçoive un pointeur vers un `struct coffre` et lise la cible via `c->cible[i]`.
- Conserver la garde de longueur du chapitre 7.

Exemple d'exécution attendu (coffre initialisé avec la cible, affichage de `verifier(&c, "Clemence")`) :

```text
1
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-10)

## Suivi

- [ ] Partie 1, chapitre 10 : lecture
- [ ] Partie 1, chapitre 10 : exercices
- [ ] Partie 1, chapitre 10 : fil rouge
