# Chapitre 13 : Projet de fin de partie

Deux travaux récapitulent toute la partie 1 : une petite bibliothèque de chaînes (synthèse), et le coffre, le mini-crackme construit tout au long des chapitres (fil rouge). Les deux se compilent avec un Makefile et réutilisent ce que vous avez écrit.

## Projet A - ma_libc (synthèse)

L'objectif est de rassembler, dans un module réutilisable, les fonctions de chaînes recodées en partie 1, et d'en ajouter deux. C'est un « rush » de synthèse typique de piscine.

- **Fichiers à rendre :** `ma_libc.h`, `ma_libc.c`, `main.c`, `Makefile`
- **Fonctions du module :** `my_strlen`, `my_strcmp`, `my_strdup`, `my_strcat`
- **Fonctions autorisées :** `write`, `malloc`, `free` (aucune fonction de `<string.h>`)

Consignes :

- `ma_libc.h` : les prototypes des quatre fonctions, avec une garde d'inclusion.
- `ma_libc.c` : leurs définitions. `my_strdup` utilise `malloc` (penser au `free` côté appelant). `my_strcat(dest, src)` ajoute `src` à la fin de `dest`, qui doit être assez grand.
- `main.c` : un test qui les exerce toutes.
- `Makefile` : construit le test et possède une règle `clean`.

Exemple d'exécution attendu (un test qui concatène `"Cle"` et `"mence"` dans un tampon assez grand) :

```text
Clemence
```

La solution complète, testée, est sur la page des solutions : [ma_libc](../solutions/partie0.md#projet-malibc).

## Projet B - le coffre (fil rouge)

Aboutissement du fil rouge : on assemble le coffre en projet propre, à partir de tout ce que vous avez écrit chapitre après chapitre. La clé valide est `Clemence`, mais, comme pour un vrai crackme, c'est la cible qui est dans le binaire, pas la clé.

- **Fichiers à rendre :** `libmy.h`, `libmy.c`, `coffre0.c`, `Makefile`
- **Fonctions autorisées :** `write`

Consignes :

- Rassembler le module `libmy` : `my_putchar`, `my_putstr`, `my_strlen` (avec en-tête et garde d'inclusion).
- Écrire `coffre0.c` : une structure `coffre` contenant la cible `{67, 109, 103, 112, 105, 115, 105, 108}`, une fonction `verifier` (garde de longueur puis vérification position par position, `cle[i] + i == cible[i]`), et un `main` qui lit `argv[1]`, affiche `Usage : ./coffre0 <cle>` si l'argument manque, puis `Coffre ouvert` ou `Refuse`.
- Écrire un `Makefile` qui construit `coffre0` et possède une règle `clean`.

Exemple d'exécution attendu :

```text
$ make
$ ./coffre0 Clemence
Coffre ouvert
$ ./coffre0 autrechose
Refuse
```

Le code complet, testé, est sur la page des solutions : [le coffre](../solutions/partie0.md#projet-coffre).

## Et ensuite

Vous savez maintenant lire et écrire du C : fonctions, variables, conditions, boucles, pointeurs, chaînes, tableaux, arguments, structures, allocation, et même un projet multi-fichiers. C'est tout ce qu'il faut pour aborder l'assembleur.

Gardez votre `coffre0` : au début de la partie 3, vous le compilerez puis le **désassemblerez**, pour retrouver, dans votre propre binaire, la cible `43 6d 67 70 69 73 69 6c` et en déduire la clé. Vous ferez alors, sur votre code, exactement le travail d'un reverser. La [partie 2](../partie1-assembleur/01-bases.md) commence par là : comment ce C se traduit en instructions machine.

## Suivi

- [ ] Projet 13 : synthèse ma_libc
- [ ] Projet 13 : mini-projet coffre
