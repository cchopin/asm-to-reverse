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

**Exercice 10.1 (N1)** Définir `struct point { int x; int y; };` puis écrire `int distance_carre(const struct point *a, const struct point *b)` qui renvoie `(ax-bx)^2 + (ay-by)^2` (on reste en entiers, sans racine). Entre `{1,2}` et `{4,6}`, le résultat est 25.

**Exercice 10.2 (N2)** Écrire `void augmente(struct point *p, int dx, int dy)` qui déplace un point en modifiant la structure via le pointeur. Vérifier l'effet dans un `main`.

**Exercice 10.3 (N2)** Définir `struct etudiant { const char *nom; int note; };` puis écrire `double moyenne(const struct etudiant *tab, int n)` qui renvoie la moyenne des notes.

**Exercice 10.4 (N3)** Ajouter `const char *meilleur(const struct etudiant *tab, int n)` qui renvoie le nom de l'étudiant ayant la meilleure note.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-10)

## Fil rouge : ranger le coffre dans une structure

On regroupe les données du coffre (sa cible) dans une structure, et `verifier` reçoit un pointeur vers ce coffre. C'est plus propre, et surtout c'est la forme qu'on retrouvera désassemblée : un pointeur vers une zone de données, lu à décalages fixes.

```c
struct coffre
{
    unsigned char cible[8];
};

int verifier(const struct coffre *c, const char *cle)
{
    if (my_strlen(cle) != 8)
        return 0;
    for (int i = 0; i < 8; i++)
        if ((unsigned char)(cle[i] + i) != c->cible[i])
            return 0;
    return 1;
}
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-10)

## Suivi

- [ ] Partie 1, chapitre 10 : lecture
- [ ] Partie 1, chapitre 10 : exercices
- [ ] Partie 1, chapitre 10 : fil rouge
