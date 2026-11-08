# Chapitre 2 : Variables et types

Une variable est une case mémoire nommée, qui contient une valeur d'un certain **type**. En reverse, le type compte énormément : la taille d'une variable et son caractère signé ou non expliquent le choix des instructions et des registres une fois le code compilé. Ce chapitre pose ces bases.

## Déclarer une variable

```c
int    n = 42;         // entier signé, 4 octets le plus souvent
char   c = 'A';        // un octet, souvent un caractère
long   g = 123456789;  // entier plus large, 8 octets
unsigned int u = 7;    // entier non signé (jamais négatif)
```

Une variable se déclare avec son type, un nom, et éventuellement une valeur initiale. Sans initialisation, sa valeur est indéterminée : ne jamais la lire avant de l'avoir écrite.

## La taille des types : sizeof

L'opérateur `sizeof` donne la taille d'un type en octets :

```c
printf("int = %zu octets\n", sizeof(int));   // souvent 4
printf("char = %zu octet\n", sizeof(char));  // toujours 1
printf("long = %zu octets\n", sizeof(long)); // souvent 8
```

`%zu` est le format pour une taille. Ces tailles se retrouvent directement en assembleur : un `int` tient dans un registre de 32 bits (`eax`), un `long` ou un pointeur dans 64 bits (`rax`).

## Opérations arithmétiques

Les opérateurs : `+`, `-`, `*`, `/` (division), `%` (reste, ou modulo). Sur des entiers, `/` est une **division entière** : `7 / 2` vaut `3`, et `7 % 2` vaut `1`. Ce couple division/modulo sert sans cesse, par exemple pour extraire des chiffres ou convertir des unités.

```c
int total = 3661;
int h = total / 3600;          // 1
int m = (total % 3600) / 60;   // 1
int s = total % 60;            // 1
```

## Signé, non signé, et débordement

Un `int` est signé : il peut être négatif. Un `unsigned int` ne le peut pas. Chaque type a une plage limitée, et dépasser cette plage provoque un **débordement** (le compteur « fait le tour »).

Un `unsigned char` va de 0 à 255. Que vaut `255 + 1` ? Zéro : on revient au début.

```c
unsigned char x = 255;
x = x + 1;            // x vaut 0, pas 256
```

Le même octet peut aussi être lu de deux façons. Un `unsigned char` de valeur 200 vaut, lu comme un `signed char`, `-56` (c'est le complément à deux, détaillé au chapitre 1 de la partie 2). C'est exactement ce genre de double lecture qui piège en reverse quand on confond un octet signé et non signé.

```c
unsigned char b = 200;
printf("%u ou %d\n", b, (signed char)b);   // 200 ou -56
```

## Afficher en hexadécimal

Le format `%x` affiche un entier en hexadécimal, la base naturelle du reverse (un octet tient sur deux chiffres hexa). On y reviendra au chapitre 1 de la partie 2, mais retenez-le dès maintenant :

```c
printf("%d en hexa : %x\n", 255, 255);   // 255 en hexa : ff
```

## Exercices

Rappel de la méthode : un fichier `.c` par exercice, la fonction demandée testée depuis votre propre `main`, et la compilation avec `gcc -Wall -Wextra`. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-02).

### Exercice 2.1 - afficher_tailles (N1)

- **Fichier à rendre :** `afficher_tailles.c`
- **Prototype :** `void afficher_tailles(void);`
- **Fonctions autorisées :** `printf`, l'opérateur `sizeof`

Afficher, une par ligne, les tailles en octets d'un `char`, d'un `int`, d'un `long`, d'un `unsigned int` et d'un pointeur (`void *`), obtenues avec `sizeof`. Notez les valeurs observées sur votre machine.

Exemple d'exécution attendu (machine 64 bits classique) :

```text
char = 1
int = 4
long = 8
unsigned int = 4
void * = 8
```

### Exercice 2.2 - trois variables (N1)

- **Fichier à rendre :** `trois_variables.c`
- **Prototype :** `void trois_variables(void);`
- **Fonctions autorisées :** `printf`

Déclarer trois variables (un `int`, un `char`, un `long`), leur donner une valeur, puis les afficher sur une seule ligne avec `printf`, en utilisant le bon format pour chacune (`%d`, `%c`, `%ld`).

Exemple d'exécution attendu (avec les valeurs `-17`, `'Z'`, `1000000000`) :

```text
-17 Z 1000000000
```

### Exercice 2.3 - hms (N2)

- **Fichier à rendre :** `hms.c`
- **Prototype :** `void hms(int total);`
- **Fonctions autorisées :** `printf`

Afficher une durée donnée en secondes sous la forme `Xh Ym Zs`. Utiliser la division entière et le modulo.

Exemple d'exécution attendu (appel `hms(3661)`) :

```text
1h 1m 1s
```

### Exercice 2.4 - apres (N2)

- **Fichier à rendre :** `apres.c`
- **Prototype :** `unsigned char apres(unsigned char x);`
- **Fonctions autorisées :** aucune

Renvoyer `x + 1`. Vérifier que `apres(255)` vaut `0`, et expliquer en commentaire pourquoi (le débordement d'un octet non signé qui « fait le tour »).

Exemple d'exécution attendu (affichage de `apres(255)` avec `%u`) :

```text
0
```

### Exercice 2.5 - interpretations (N3)

- **Fichier à rendre :** `interpretations.c`
- **Prototype :** `void interpretations(unsigned char b);`
- **Fonctions autorisées :** `printf`

Afficher la valeur de `b` lue comme non signée (`%u`), puis la même donnée lue comme signée (`%d` après conversion en `signed char`). Testez avec `0`, `127`, `128`, `200` et `255`, et expliquez les résultats en commentaire.

Exemple d'exécution attendu (appel `interpretations(200)`) :

```text
200 -> -56
```

## Fil rouge : la cible du coffre

Le coffre vérifiera la clé en la comparant à une **cible** : huit octets stockés dans le programme. On les introduit maintenant, sans encore s'en servir.

À vous de compléter `coffre0` :

- **Fichier à rendre :** `coffre0.c` (celui commencé au chapitre 1)
- **Fonctions autorisées :** `printf`

Consignes :

- Déclarer un tableau `unsigned char cible[8]` contenant les huit octets suivants : 67, 109, 103, 112, 105, 115, 105, 108.
- L'afficher en hexadécimal, chaque octet sur deux chiffres, séparés par une espace, suivi d'un retour à la ligne.

Indice : le format `%02x` affiche un octet sur deux chiffres hexadécimaux (avec un zéro devant si besoin). Ces octets sont ce qu'un reverser verrait dans le binaire : en partie 3, vous retrouverez exactement ce tableau dans votre propre programme désassemblé, et il faudra en déduire la clé.

Exemple d'exécution attendu :

```text
43 6d 67 70 69 73 69 6c
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-02)

## Suivi

- [ ] Partie 1, chapitre 2 : lecture
- [ ] Partie 1, chapitre 2 : exercices
- [ ] Partie 1, chapitre 2 : fil rouge
