# Chapitre 1 : Compiler et afficher

Bienvenue dans la partie 1, l'initiation au langage C. C'est une piscine : beaucoup de pratique, des exercices gradués, une montée en difficulté régulière. L'objectif n'est pas de devenir développeur C, mais de savoir lire du C et de maîtriser les notions qui servent au reverse (adresses, pointeurs, pile, chaînes, tableaux, structures). Tout le reste du cours s'appuie dessus.

Elle peut être sautée par ceux qui connaissent déjà bien le C, mais la série d'exercices vaut le détour même pour réviser. Pour les autres, elle se complète avec la ressource en français citée dans les [prérequis](../README.md#prerequis).

!!! note "De quoi a-t-on besoin pour cette partie 1 ?"
    Juste d'un compilateur C. Deux possibilités :

    - **Le compilateur de votre machine**, suffisant ici et plus simple à ce stade. Sous Linux, `gcc` est fourni par le paquet `build-essential` (Debian, Ubuntu), `gcc` (Fedora) ou `base-devel` (Arch) ; sur Mac, voici la marche à suivre. S'il n'est pas déjà là, l'installer une fois avec `xcode-select --install`, puis utiliser la commande `cc` (ou `gcc`) dans le Terminal. Les exemples (`write`, `unistd.h`) fonctionnent aussi sous macOS.
    - **Le conteneur Docker du cours**, si vous l'avez déjà installé (voir le guide [Installation sur Mac](../environnement/INSTALLATION-MAC.md) ou [Installation sous Linux](../environnement/INSTALLATION-LINUX.md)). Il est de toute façon nécessaire à partir de la partie 2, mais pas indispensable pour cette partie 1.

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

`gcc` traduit le C en exécutable. L'option `-o programme` donne le nom du résultat. Les options de compilation utiles seront vues plus tard (le chapitre 2 de la partie 2 en dresse la liste).

## Afficher un caractère sans la libc

Pour afficher, on utilise d'abord l'appel système `write` (on le reverra en détail bien plus tard, au chapitre 1 de la partie 4), qui écrit des octets sur la sortie standard (descripteur 1). Il faut inclure son en-tête avec `#include`, qui rend disponibles des fonctions définies ailleurs :

```c
#include <unistd.h>

int main(void)
{
    char c = 'A';
    write(1, &c, 1);   // écrire 1 octet, à l'adresse de c, sur la sortie 1
    return 0;
}
```

`&c` est l'**adresse** de la variable `c` : `write` a besoin de savoir *où* se trouve l'octet à écrire, pas seulement sa valeur. Cette notion d'adresse est au cœur du C et du reverse ; elle est détaillée au chapitre 6.

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

On le compile et on l'exécute comme précédemment. Dans les exemples suivants, on ne répètera pas le `main` ni les `#include` à chaque fois, mais ils restent nécessaires : chaque fonction se place au-dessus d'un `main` qui l'appelle.

## Un caractère est un petit entier

En C, le type `char` est un entier sur un octet. Le caractère `'A'` vaut en réalité 65 (son code ASCII), `'0'` vaut 48, et les lettres comme les chiffres se suivent dans la table. Deux conséquences très utiles :

- `'a' + 1` vaut `'b'` : on peut parcourir l'alphabet en ajoutant 1 ;
- `'0' + 7` vaut `'7'` : on obtient le caractère d'un chiffre en l'ajoutant à `'0'`.

## Répéter avec une boucle

Pour afficher plusieurs caractères, on répète avec une boucle `for`. Les boucles sont détaillées au chapitre 4 ; on en utilise ici la forme la plus simple :

```c
for (char c = 'a'; c <= 'z'; c++)   // de 'a' à 'z'
    my_putchar(c);                  // afficher chaque lettre
```

`c++` ajoute 1 à `c` à chaque tour. La boucle s'arrête dès que la condition `c <= 'z'` devient fausse.

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

`%d` est remplacé par l'entier donné ensuite. Les formats utiles : `%d` (entier), `%c` (caractère), `%s` (chaîne), `%p` (adresse), `%x` (hexadécimal). `printf` reviendra souvent, y compris pour comprendre comment il apparaît en assembleur (chapitre 2 de la partie 4).

## Exercices

Chaque exercice se présente comme un petit sujet : un **fichier à rendre**, un **prototype** à respecter, les **fonctions autorisées**, des consignes, et l'exécution attendue. Écrivez la fonction demandée, puis testez-la depuis un `main` de votre cru (le fil rouge, lui, est un programme complet). Compilez toujours avec les avertissements activés :

```bash
gcc -Wall -Wextra fichier.c -o fichier && ./fichier
```

Les niveaux indiquent la difficulté : **N1** découverte, **N2** consolidation, **N3** défi. Les solutions sont regroupées à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-01) : ne les ouvrez qu'après avoir cherché par vous-même.

### Exercice 1.1 - my_putchar (N1)

- **Fichier à rendre :** `my_putchar.c`
- **Prototype :** `void my_putchar(char c);`
- **Fonctions autorisées :** `write`

Afficher le caractère `c` sur la sortie standard à l'aide de l'appel système `write`. C'est la brique de base : toutes les fonctions d'affichage des exercices suivants s'appuieront dessus.

Exemple, avec un `main` qui appelle `my_putchar('H')`, `my_putchar('i')`, puis `my_putchar('\n')` :

```text
Hi
```

### Exercice 1.2 - afficher 42 (N1)

- **Fichier à rendre :** `afficher_42.c`
- **Prototype :** `void afficher_42(void);`
- **Fonctions autorisées :** `my_putchar` (votre fonction du 1.1)

Afficher les deux caractères `4` et `2`, puis un retour à la ligne. L'usage de `printf` est interdit : le but est de n'écrire que des caractères, un par un.

Exemple d'exécution attendu :

```text
42
```

### Exercice 1.3 - mon prénom (N1)

- **Fichier à rendre :** `mon_prenom.c`
- **Prototype :** `void mon_prenom(void);`
- **Fonctions autorisées :** `my_putchar`

Afficher votre prénom suivi d'un retour à la ligne, un caractère à la fois.

Exemple d'exécution attendu (pour le prénom Ada) :

```text
Ada
```

### Exercice 1.4 - les chiffres (N2)

- **Fichier à rendre :** `chiffres.c`
- **Prototype :** `void chiffres(void);`
- **Fonctions autorisées :** `my_putchar`, `printf`

Afficher `0123456789` sur une seule ligne. Ensuite, dans votre `main`, afficher la valeur numérique du caractère `'0'` avec `printf("%d\n", '0');` et vérifier qu'elle vaut 48.

Exemple d'exécution attendu :

```text
0123456789
48
```

### Exercice 1.5 - l'alphabet (N2)

- **Fichier à rendre :** `alphabet.c`
- **Prototype :** `void alphabet(void);`
- **Fonctions autorisées :** `my_putchar`

Afficher `abcdefghijklmnopqrstuvwxyz` suivi d'un retour à la ligne, à l'aide d'une boucle.

Exemple d'exécution attendu :

```text
abcdefghijklmnopqrstuvwxyz
```

### Exercice 1.6 - l'alphabet à l'envers (N3)

- **Fichier à rendre :** `alphabet_inverse.c`
- **Prototypes :** `void alphabet_inverse(void);` et, en bonus, `void une_lettre_sur_deux(void);`
- **Fonctions autorisées :** `my_putchar`

Écrire `alphabet_inverse`, qui affiche `zyxwvutsrqponmlkjihgfedcba`. En bonus, écrire `une_lettre_sur_deux`, qui affiche `acegikmoqsuwy` (une lettre sur deux, à partir de `a`).

Exemple d'exécution attendu (les deux fonctions à la suite) :

```text
zyxwvutsrqponmlkjihgfedcba
acegikmoqsuwy
```

## Fil rouge : le coffre

Le fil rouge de cette partie 1 est un petit programme que l'on construit chapitre après chapitre : **coffre0**, un mini-crackme. Il demandera une clé et répondra si elle ouvre le coffre. À la fin de la partie, il sera complet, et on le réutilisera au début de la partie 3 pour le désassembler : vous analyserez votre propre programme.

- **Fichier à rendre :** `coffre0.c` (c'est le fichier que l'on complétera à chaque chapitre ; gardez-le précieusement)
- **Fonctions autorisées :** `printf`

Pour ce premier chapitre, on pose seulement le décor. Écrire un programme `coffre0` dont le `main` affiche exactement, avec `printf`, ces deux lignes :

```text
=== Coffre 0 ===
Usage : ./coffre0 <cle>
```

C'est tout pour l'instant. Les chapitres suivants ajouteront la lecture de la clé, la vérification, puis la logique complète. À chaque étape, vous écrivez le code vous-même ; la solution n'est là que pour vérifier après coup.

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-01)

## Suivi

- [ ] Partie 1, chapitre 1 : lecture
- [ ] Partie 1, chapitre 1 : exercices
- [ ] Partie 1, chapitre 1 : fil rouge
