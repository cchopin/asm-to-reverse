# Chapitre 7 : Chaînes de caractères

Une chaîne est partout dans un crackme : un mot de passe, un message, un nom comparé à un autre. Ce chapitre est une vraie série « piscine » : on recode à la main les fonctions de base sur les chaînes, sans la bibliothèque standard. C'est le meilleur entraînement pour reconnaître ces mêmes routines une fois désassemblées.

## Une chaîne est un tableau terminé par zéro

En C, une chaîne est un **tableau de caractères** terminé par un octet nul `'\0'`. Il n'existe pas de type « chaîne » : on manipule une chaîne par un pointeur `char *` vers son premier caractère.

```c
char *s = "Clemence";
```

En mémoire : `'C'`, `'l'`, `'e'`, `'m'`, `'e'`, `'n'`, `'c'`, `'e'`, `'\0'`. Le `'\0'` final (octet de valeur 0) marque la fin : c'est lui qui permet aux fonctions de savoir où la chaîne s'arrête. Oublier ce terminateur est une source classique de bugs et de failles.

On parcourt une chaîne en avançant jusqu'au `'\0'` :

```c
int i = 0;
while (s[i] != '\0')
    i++;
```

`s[i]` est équivalent à `*(s + i)` : on part de l'adresse de la chaîne, on avance de `i`, on déréférence (chapitre 6).

## Chaîne littérale et tableau modifiable

Une chaîne littérale (`"..."`) ne doit pas être modifiée. Pour une chaîne que l'on va changer, on utilise un tableau :

```c
char mot[] = "reverse";   // copie modifiable, de taille 8 (7 lettres + '\0')
mot[0] = 'R';             // autorisé
```

## Exercices

Série de fonctions à recoder, dans un fichier `.c` chacune, testées depuis votre propre `main` et compilées avec `gcc -Wall -Wextra`. Les contraintes sont volontairement strictes, comme en piscine. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-07).

!!! warning "Contraintes de la série"
    Dans tous ces exercices, les fonctions de `<string.h>` (`strlen`, `strcmp`, `strcpy`...) sont **interdites** : c'est tout l'intérêt de les réécrire. Les « fonctions autorisées » de chaque sujet se limitent à `my_putchar` et, quand c'est précisé, à vos propres fonctions des exercices précédents.

### Exercice 7.1 - my_strlen (N1)

- **Fichier à rendre :** `my_strlen.c`
- **Prototype :** `int my_strlen(const char *s);`
- **Fonctions autorisées :** aucune

Renvoyer la longueur d'une chaîne (sans compter le `'\0'`).

Exemple d'exécution attendu (appel `my_strlen("Clemence")`) :

```text
8
```

### Exercice 7.2 - my_putstr (N1)

- **Fichier à rendre :** `my_putstr.c`
- **Prototype :** `void my_putstr(const char *s);`
- **Fonctions autorisées :** `my_putchar`

Afficher une chaîne caractère par caractère.

Exemple d'exécution attendu (appel `my_putstr("reverse\n")`) :

```text
reverse
```

### Exercice 7.3 - my_strcmp (N2)

- **Fichier à rendre :** `my_strcmp.c`
- **Prototype :** `int my_strcmp(const char *a, const char *b);`
- **Fonctions autorisées :** aucune

Renvoyer 0 si les chaînes sont égales, un nombre négatif si `a` est « avant » `b`, positif sinon. Comparer caractère par caractère, en s'arrêtant à la première différence ou au `'\0'`.

Exemple d'exécution attendu (appels `my_strcmp("abc", "abc")`, `my_strcmp("abc", "abd")`) :

```text
0 negatif
```

### Exercice 7.4 - my_strcpy (N2)

- **Fichier à rendre :** `my_strcpy.c`
- **Prototype :** `char *my_strcpy(char *dest, const char *src);`
- **Fonctions autorisées :** aucune

Copier `src` dans `dest` (terminateur compris) et renvoyer `dest`. On suppose `dest` assez grand.

Exemple d'exécution attendu (copie de `"reverse"` puis affichage de `dest`) :

```text
reverse
```

### Exercice 7.5 - my_revstr (N2)

- **Fichier à rendre :** `my_revstr.c`
- **Prototype :** `char *my_revstr(char *s);`
- **Fonctions autorisées :** aucune

Inverser une chaîne sur place (sans tableau intermédiaire) et renvoyer `s`. Indice : deux indices, l'un au début, l'autre à la fin, qui avancent l'un vers l'autre en échangeant.

Exemple d'exécution attendu (appel sur `"assembleur"`) :

```text
ruelbmessa
```

### Exercice 7.6 - est_palindrome (N3)

- **Fichier à rendre :** `est_palindrome.c`
- **Prototype :** `int est_palindrome(const char *s);`
- **Fonctions autorisées :** aucune

Renvoyer 1 si la chaîne se lit pareil dans les deux sens, 0 sinon.

Exemple d'exécution attendu (appels `est_palindrome("kayak")`, `est_palindrome("reverse")`) :

```text
1 0
```

### Exercice 7.7 - my_upcase (N3)

- **Fichier à rendre :** `my_upcase.c`
- **Prototype :** `void my_upcase(char *s);`
- **Fonctions autorisées :** aucune

Mettre toutes les lettres minuscules en majuscules, sur place. Indice : une lettre minuscule et sa majuscule diffèrent de 32 dans la table ASCII.

Exemple d'exécution attendu (appel sur `"reverse 42"`) :

```text
REVERSE 42
```

## Fil rouge : vérifier la longueur

Un bon crackme rejette d'emblée une clé de mauvaise taille. On ajoute cette garde en tête de `verifier`, avec le `my_strlen` qu'on vient d'écrire.

À vous de compléter `coffre0` :

- **Fichier à rendre :** `coffre0.c`
- **Prototype :** `int verifier(const char *cle);`
- **Fonctions autorisées :** `my_strlen` (votre fonction du 7.1)

Consignes :

- Ajouter, en tête de `verifier`, un test : si la longueur de `cle` n'est pas 8, renvoyer 0 immédiatement.
- Conserver ensuite la boucle de vérification position par position.

Sans cette garde, une clé plus courte que 8 ferait lire la fonction au-delà du `'\0'`, en mémoire quelconque : exactement le genre de défaut qu'on cherche, plus tard, à exploiter.

Exemple d'exécution attendu (affichage de `verifier("Clem")` puis `verifier("Clemence")`) :

```text
0 1
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-07)

## Suivi

- [ ] Partie 1, chapitre 7 : lecture
- [ ] Partie 1, chapitre 7 : exercices
- [ ] Partie 1, chapitre 7 : fil rouge
