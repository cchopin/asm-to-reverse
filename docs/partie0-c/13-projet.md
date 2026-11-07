# 0.13 Projet de fin de partie

Deux travaux récapitulent toute la partie 0 : une petite bibliothèque de chaînes (synthèse), et le coffre, le mini-crackme construit tout au long des chapitres (fil rouge). Les deux se compilent avec un Makefile et réutilisent ce que vous avez écrit.

## Synthèse : ma_libc

L'objectif est de rassembler, dans un module réutilisable, les fonctions de chaînes recodées en partie 0, et d'en ajouter deux. C'est un « rush » de synthèse typique de piscine.

À réaliser :

- `ma_libc.h` : les prototypes de `my_strlen`, `my_strcmp`, `my_strdup`, `my_strcat`, avec une garde d'inclusion.
- `ma_libc.c` : leurs définitions.
- un `main.c` de test qui les exerce toutes.
- un `Makefile` qui construit le test et possède une règle `clean`.

!!! warning "Contraintes"
    Aucune fonction de `<string.h>`. `my_strdup` utilise `malloc` (penser au `free`). `my_strcat(dest, src)` ajoute `src` à la fin de `dest`, qui doit être assez grand.

La solution complète, testée, est sur la page des solutions : [ma_libc](../solutions/partie0.md#projet-malibc).

## Mini-projet : le coffre

Voici le coffre complet, aboutissement du fil rouge. Il réutilise `libmy` et range sa cible dans une structure. La clé valide est `Clemence`, mais, comme pour un vrai crackme, c'est la cible qui est dans le binaire, pas la clé.

`libmy.h`

```c
#ifndef LIBMY_H
#define LIBMY_H

void my_putchar(char c);
void my_putstr(const char *s);
int  my_strlen(const char *s);

#endif
```

`libmy.c`

```c
#include <unistd.h>
#include "libmy.h"

void my_putchar(char c) { write(1, &c, 1); }
void my_putstr(const char *s) { for (int i = 0; s[i]; i++) my_putchar(s[i]); }
int  my_strlen(const char *s) { int i = 0; while (s[i]) i++; return i; }
```

`coffre0.c`

```c
#include "libmy.h"

struct coffre
{
    unsigned char cible[8];
};

static int verifier(const struct coffre *c, const char *cle)
{
    if (my_strlen(cle) != 8)
        return 0;
    for (int i = 0; i < 8; i++)
        if ((unsigned char)(cle[i] + i) != c->cible[i])
            return 0;
    return 1;
}

int main(int argc, char **argv)
{
    struct coffre c = {{67, 109, 103, 112, 105, 115, 105, 108}};

    if (argc != 2) {
        my_putstr("Usage : ./coffre0 <cle>\n");
        return 1;
    }
    if (verifier(&c, argv[1]))
        my_putstr("Coffre ouvert\n");
    else
        my_putstr("Refuse\n");
    return 0;
}
```

`Makefile`

```make
CC = gcc
CFLAGS = -Wall -Wextra

coffre0: coffre0.o libmy.o
	$(CC) $(CFLAGS) -o coffre0 coffre0.o libmy.o

coffre0.o: coffre0.c libmy.h
	$(CC) $(CFLAGS) -c coffre0.c

libmy.o: libmy.c libmy.h
	$(CC) $(CFLAGS) -c libmy.c

clean:
	rm -f *.o coffre0

.PHONY: clean
```

Construction et essais :

```bash
make
./coffre0 Clemence     # Coffre ouvert
./coffre0 autrechose   # Refuse
```

## Et ensuite

Vous savez maintenant lire et écrire du C : fonctions, variables, conditions, boucles, pointeurs, chaînes, tableaux, arguments, structures, allocation, et même un projet multi-fichiers. C'est tout ce qu'il faut pour aborder l'assembleur.

Gardez votre `coffre0` : au début de la partie 2, vous le compilerez puis le **désassemblerez**, pour retrouver, dans votre propre binaire, la cible `43 6d 67 70 69 73 69 6c` et en déduire la clé. Vous ferez alors, sur votre code, exactement le travail d'un reverser. La [partie 1](../partie1-assembleur/01-bases.md) commence par là : comment ce C se traduit en instructions machine.

## Suivi

- [ ] Projet 0.13 : synthèse ma_libc
- [ ] Projet 0.13 : mini-projet coffre
