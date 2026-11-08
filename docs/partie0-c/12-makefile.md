# Chapitre 12 : En-têtes, compilation séparée et Makefile

Un vrai programme tient rarement dans un seul fichier. Ce chapitre montre comment découper le code en plusieurs fichiers, les relier avec des en-têtes, et automatiser la compilation avec un Makefile. C'est aussi utile côté reverse : comprendre comment un binaire est assemblé à partir de plusieurs modules aide à lire la table des symboles et à séparer le code de l'auteur de celui des bibliothèques.

## Fichiers d'en-tête

Un fichier `.h` contient les **prototypes** (chapitre 5) des fonctions qu'un module expose. Les autres fichiers l'incluent pour pouvoir les appeler.

```c
/* libmy.h */
#ifndef LIBMY_H
#define LIBMY_H

void my_putchar(char c);
void my_putstr(const char *s);
int  my_strlen(const char *s);

#endif
```

Les lignes `#ifndef / #define / #endif` forment une **garde d'inclusion** : elles évitent que le contenu soit inclus deux fois. On met dans le `.h` les prototypes, pas le corps des fonctions.

## Compilation séparée

Le corps des fonctions va dans un `.c` qui inclut son `.h` :

```c
/* libmy.c */
#include <unistd.h>
#include "libmy.h"

void my_putchar(char c) { write(1, &c, 1); }
void my_putstr(const char *s) { for (int i = 0; s[i]; i++) my_putchar(s[i]); }
int  my_strlen(const char *s) { int i = 0; while (s[i]) i++; return i; }
```

Un autre fichier l'utilise en incluant le `.h` :

```c
/* main.c */
#include "libmy.h"

int main(void) { my_putstr("salut\n"); return 0; }
```

On compile en deux temps : chaque `.c` donne un fichier objet `.o`, puis on relie les `.o` en un exécutable.

```bash
gcc -Wall -Wextra -c libmy.c     # produit libmy.o
gcc -Wall -Wextra -c main.c      # produit main.o
gcc -o prog main.o libmy.o       # édition de liens
```

`#include "libmy.h"` (guillemets) cherche le fichier à côté du source ; `#include <stdio.h>` (chevrons) cherche dans les en-têtes système.

## Makefile

Retaper ces commandes à chaque fois est pénible. Un `Makefile` décrit les règles, et `make` ne recompile que ce qui a changé.

```make
CC = gcc
CFLAGS = -Wall -Wextra

prog: main.o libmy.o
	$(CC) $(CFLAGS) -o prog main.o libmy.o

main.o: main.c libmy.h
	$(CC) $(CFLAGS) -c main.c

libmy.o: libmy.c libmy.h
	$(CC) $(CFLAGS) -c libmy.c

clean:
	rm -f *.o prog

.PHONY: clean
```

Chaque règle a une **cible**, ses **dépendances**, et une **commande** (précédée d'une vraie tabulation, pas d'espaces, sinon `make` se plaint). On lance `make` pour construire, `make clean` pour nettoyer.

## Exercices

Ces exercices se suivent : on construit un petit module et son `Makefile`. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-12).

### Exercice 12.1 - un module libmy (N1)

- **Fichiers à rendre :** `libmy.h`, `libmy.c`, `main.c`
- **Fonctions du module :** `my_putchar`, `my_putstr`, `my_strlen`
- **Fonctions autorisées :** `write`

Répartir `my_putchar`, `my_putstr` et `my_strlen` dans `libmy.c` et `libmy.h` (avec garde d'inclusion), et écrire un `main.c` qui les utilise. Compiler à la main en passant par les `.o`.

Exemple d'exécution attendu (un `main` qui appelle `my_putstr("salut\n")`) :

```text
salut
```

### Exercice 12.2 - un Makefile (N2)

- **Fichier à rendre :** `Makefile`
- **Fonctions autorisées :** `make`, `gcc`

Écrire un `Makefile` qui construit le programme du 12.1, avec une règle `clean`. Vérifier que `make` ne recompile que les fichiers modifiés (attention : les commandes doivent commencer par une vraie tabulation).

Exemple d'exécution attendu :

```text
$ make
gcc -Wall -Wextra -c main.c
gcc -Wall -Wextra -c libmy.c
gcc -Wall -Wextra -o prog main.o libmy.o
$ make
make: 'prog' is up to date.
```

### Exercice 12.3 - étendre le module (N3)

- **Fichiers à rendre :** `libmy.h`, `libmy.c` (mis à jour)
- **Fonctions autorisées :** `write`

Ajouter au module une ou deux fonctions de votre choix (par exemple `my_putnbr`), mettre à jour le `.h`, et constater que `make` ne recompile que les fichiers concernés.

!!! tip "Le manuel"
    Les fonctions de la libc ont un manuel : `man 3 printf`, `man 3 malloc`, `man 2 write` (la section 2 regroupe les appels système, la section 3 les fonctions de bibliothèque). Savoir lire un `man` fait partie du métier.

## Fil rouge : le coffre en projet

On assemble le coffre en projet propre : `libmy.c` / `libmy.h` pour les utilitaires, `coffre0.c` pour la logique, un `Makefile` pour construire le tout. Le code complet et testé est donné au chapitre suivant, [le projet de fin de partie](13-projet.md).

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-12)

## Suivi

- [ ] Partie 1, chapitre 12 : lecture
- [ ] Partie 1, chapitre 12 : exercices
- [ ] Partie 1, chapitre 12 : fil rouge
