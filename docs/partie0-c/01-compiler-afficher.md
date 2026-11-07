# 0.1 Compiler et afficher

Cette partie 0 est une initiation au langage C, destinée à ceux qui en ont peu ou pas fait. Elle n'a pas pour but de former des développeurs C, mais de donner les bases strictement utiles au reverse : lire du C, comprendre les pointeurs, les chaînes, les tableaux, les structures et les arguments de la ligne de commande. Tout le reste du cours (lecture de l'assembleur) s'appuie dessus.

Elle peut être sautée par ceux qui connaissent déjà le C. Pour les autres, elle se complète très bien avec la ressource en français citée dans les [prérequis](../README.md#prerequis). Les exercices sont dans l'esprit des « piscines » de programmation : courts, progressifs, à écrire soi-même. On commence sans la bibliothèque standard, pour bien voir ce qui se passe.

!!! note "De quoi a-t-on besoin pour cette partie 0 ?"
    Juste d'un compilateur C. Deux possibilités :

    - **Le compilateur du Mac**, suffisant ici et plus simple à ce stade. S'il n'est pas déjà là, l'installer une fois avec `xcode-select --install`, puis utiliser la commande `cc` (ou `gcc`) dans le Terminal. Les exemples (`write`, `unistd.h`) fonctionnent aussi sous macOS.
    - **Le conteneur Docker du cours**, si vous l'avez déjà installé (voir le guide [Installation sur Mac](../environnement/INSTALLATION-MAC.md)). Il est de toute façon nécessaire à partir du chapitre 1, mais pas indispensable pour cette partie 0.

    Dans la suite, `gcc` et `cc` sont interchangeables ; utilisez celui dont vous disposez.

## Un premier programme

Un programme C minimal :

```c
int main(void)
{
    return 0;
}
```

- `main` est la fonction par laquelle tout commence.
- `int` est le **type** de ce que `main` renvoie : un entier. `return 0;` signifie « terminé sans erreur ». Cette valeur est le **code de retour** du programme ; dans le Terminal, la commande `echo $?` affiche le code de retour du dernier programme lancé.
- `void` entre les parenthèses signifie « aucun argument ».

On écrit ce code dans un fichier texte nommé `programme.c` (avec l'éditeur de votre choix sur le Mac, ou `nano programme.c` dans un terminal). Puis, dans le Terminal, depuis le dossier qui contient ce fichier :

```bash
gcc programme.c -o programme   # ou : cc programme.c -o programme
./programme
echo $?        # affiche 0
```

`gcc` traduit le C en exécutable. L'option `-o programme` donne le nom du résultat. Les options de compilation utiles seront vues plus tard (le chapitre 2 en dresse la liste).

## Afficher un caractère sans la libc

Pour afficher, on utilise d'abord l'appel système `write` (on le reverra en détail bien plus tard, au chapitre 15), qui écrit des octets sur la sortie standard (descripteur 1). Il faut inclure son en-tête avec `#include`, qui rend disponibles des fonctions définies ailleurs :

```c
#include <unistd.h>

int main(void)
{
    char c = 'A';
    write(1, &c, 1);   // écrire 1 octet, à l'adresse de c, sur la sortie 1
    return 0;
}
```

`&c` est l'**adresse** de la variable `c` : `write` a besoin de savoir *où* se trouve l'octet à écrire, pas seulement sa valeur. Cette notion d'adresse est au cœur du C et du reverse ; elle est détaillée en 0.3.

Pour éviter de répéter `write`, on écrit une petite fonction, exactement comme le fera le compilateur :

```c
#include <unistd.h>

void my_putchar(char c)
{
    write(1, &c, 1);
}
```

`void` signifie que la fonction ne renvoie rien. La fonction et le `main` vont dans **le même fichier**, la fonction étant définie **au-dessus** du `main` qui l'appelle. Voici donc le fichier complet, prêt à compiler :

```c
#include <unistd.h>

void my_putchar(char c)
{
    write(1, &c, 1);
}

int main(void)
{
    my_putchar('H');
    my_putchar('i');
    my_putchar('\n');   // retour à la ligne
    return 0;
}
```

On le compile et on l'exécute comme précédemment (`gcc fichier.c -o fichier`, puis `./fichier`). Dans les exemples suivants, on ne répètera pas le `main` ni les `#include` à chaque fois, mais ils restent nécessaires : chaque fonction se place au-dessus d'un `main` qui l'appelle.

## Afficher avec printf

Une fois les bases en place, `printf` (de la bibliothèque standard) est plus pratique :

```c
#include <stdio.h>

int main(void)
{
    printf("La reponse est %d\n", 42);
    return 0;
}
```

`%d` est remplacé par l'entier donné ensuite. Les formats utiles : `%d` (entier), `%c` (caractère), `%s` (chaîne), `%p` (adresse), `%x` (hexadécimal). `printf` reviendra souvent, y compris pour comprendre comment il apparaît en assembleur (chapitre 16).

## Exercices

Écrire et compiler chaque fonction, et la tester depuis un `main`.

**Exercice 0.1.1** Écrire `void my_putchar(char c)` qui affiche un caractère avec `write`. C'est la brique de base des exercices suivants.

**Exercice 0.1.2** Écrire `void alphabet(void)` qui affiche `abcdefghijklmnopqrstuvwxyz` suivi d'un retour à la ligne, en utilisant `my_putchar` dans une boucle. Indice : en C, un `char` est un petit entier ; `'a' + 1` vaut `'b'`.

**Exercice 0.1.3** Écrire `void chiffres(void)` qui affiche `0123456789`. Afficher ensuite la valeur numérique du caractère `'0'` avec `printf("%d\n", '0');` : un caractère est en réalité un petit entier, et cette valeur (son code ASCII) explique l'astuce `'0' + n` pour obtenir le caractère d'un chiffre.

**Exercice 0.1.4** Sans utiliser `printf`, afficher la ligne `42` (les deux caractères `'4'` et `'2'`, puis un retour à la ligne) avec `my_putchar` uniquement.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-01)

## Suivi

- [ ] Chapitre 0.1 : lecture
- [ ] Chapitre 0.1 : exercices
