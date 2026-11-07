# 0.1 Compiler et afficher

Bienvenue dans la partie 0, l'initiation au langage C. C'est une piscine : beaucoup de pratique, des exercices gradués, une montée en difficulté régulière. L'objectif n'est pas de devenir développeur C, mais de savoir lire du C et de maîtriser les notions qui servent au reverse (adresses, pointeurs, pile, chaînes, tableaux, structures). Tout le reste du cours s'appuie dessus.

Elle peut être sautée par ceux qui connaissent déjà bien le C, mais la série d'exercices vaut le détour même pour réviser. Pour les autres, elle se complète avec la ressource en français citée dans les [prérequis](../README.md#prerequis).

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

`&c` est l'**adresse** de la variable `c` : `write` a besoin de savoir *où* se trouve l'octet à écrire, pas seulement sa valeur. Cette notion d'adresse est au cœur du C et du reverse ; elle est détaillée au chapitre 0.6.

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

Pour afficher plusieurs caractères, on répète avec une boucle `for`. Les boucles sont détaillées au chapitre 0.4 ; on en utilise ici la forme la plus simple :

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

`%d` est remplacé par l'entier donné ensuite. Les formats utiles : `%d` (entier), `%c` (caractère), `%s` (chaîne), `%p` (adresse), `%x` (hexadécimal). `printf` reviendra souvent, y compris pour comprendre comment il apparaît en assembleur (chapitre 16).

## Exercices

Écrire chaque fonction, la compiler et la tester depuis un `main`. Les niveaux indiquent la difficulté : **N1** découverte, **N2** consolidation, **N3** défi.

**Exercice 0.1.1 (N1)** Écrire `void my_putchar(char c)` qui affiche un caractère avec `write`. C'est la brique de base de tous les exercices suivants.

**Exercice 0.1.2 (N1)** Sans utiliser `printf`, afficher la ligne `42` (les deux caractères `'4'` et `'2'`, puis un retour à la ligne) avec `my_putchar` uniquement.

**Exercice 0.1.3 (N1)** Afficher votre prénom suivi d'un retour à la ligne, un caractère à la fois avec `my_putchar`.

**Exercice 0.1.4 (N2)** Écrire `void chiffres(void)` qui affiche `0123456789`. Afficher ensuite la valeur numérique du caractère `'0'` avec `printf("%d\n", '0');` et vérifier que c'est bien 48.

**Exercice 0.1.5 (N2)** Écrire `void alphabet(void)` qui affiche `abcdefghijklmnopqrstuvwxyz` suivi d'un retour à la ligne, avec une boucle et `my_putchar`.

**Exercice 0.1.6 (N3)** Écrire `void alphabet_inverse(void)` qui affiche `zyxwvutsrqponmlkjihgfedcba`. Puis, en bonus, écrire `void une_lettre_sur_deux(void)` qui affiche `acegikmoqsuwy`.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-01)

## Fil rouge : le coffre

Le fil rouge de cette partie 0 est un petit programme que l'on construit chapitre après chapitre : **coffre0**, un mini-crackme. Il demandera une clé et répondra si elle ouvre le coffre. À la fin de la partie, il sera complet, et on le réutilisera au début de la partie 2 pour le désassembler : vous analyserez votre propre programme.

Pour ce premier chapitre, on pose seulement le décor. Écrire un programme `coffre0` qui affiche, avec `printf` :

```text
=== Coffre 0 ===
Usage : ./coffre0 <cle>
```

C'est tout pour l'instant. Les chapitres suivants ajouteront la lecture de la clé, la vérification, puis la logique complète.

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-01)

## Suivi

- [ ] Chapitre 0.1 : lecture
- [ ] Chapitre 0.1 : exercices
- [ ] Chapitre 0.1 : fil rouge
