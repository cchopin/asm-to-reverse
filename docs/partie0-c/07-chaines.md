# 0.7 Chaînes de caractères

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

`s[i]` est équivalent à `*(s + i)` : on part de l'adresse de la chaîne, on avance de `i`, on déréférence (chapitre 0.6).

## Chaîne littérale et tableau modifiable

Une chaîne littérale (`"..."`) ne doit pas être modifiée. Pour une chaîne que l'on va changer, on utilise un tableau :

```c
char mot[] = "reverse";   // copie modifiable, de taille 8 (7 lettres + '\0')
mot[0] = 'R';             // autorisé
```

## Exercices

Série de fonctions à recoder. Les contraintes sont volontairement strictes, comme en piscine.

!!! warning "Contraintes"
    Dans tous ces exercices, les fonctions de `<string.h>` (`strlen`, `strcmp`, `strcpy`...) sont **interdites** : c'est tout l'intérêt de les réécrire. Seuls `my_putchar` et, quand c'est précisé, vos propres fonctions des exercices précédents sont autorisés.

**Exercice 0.7.1 (N1)** Écrire `int my_strlen(const char *s)` qui renvoie la longueur d'une chaîne. Vérifier sur `"Clemence"` (8).

**Exercice 0.7.2 (N1)** Écrire `void my_putstr(const char *s)` qui affiche une chaîne caractère par caractère avec `my_putchar`.

**Exercice 0.7.3 (N2)** Écrire `int my_strcmp(const char *a, const char *b)` qui renvoie 0 si les chaînes sont égales, un nombre négatif si `a` est « avant » `b`, positif sinon (comparer caractère par caractère, s'arrêter à la première différence ou au `'\0'`).

**Exercice 0.7.4 (N2)** Écrire `char *my_strcpy(char *dest, const char *src)` qui copie `src` dans `dest` (terminateur compris) et renvoie `dest`. On suppose `dest` assez grand.

**Exercice 0.7.5 (N2)** Écrire `char *my_revstr(char *s)` qui inverse une chaîne sur place (sans tableau intermédiaire) et renvoie `s`. Indice : deux indices, l'un au début, l'autre à la fin, qui avancent l'un vers l'autre en échangeant. `"assembleur"` devient `"ruelbmessa"`.

**Exercice 0.7.6 (N3)** Écrire `int est_palindrome(const char *s)` qui renvoie 1 si la chaîne se lit pareil dans les deux sens (`"kayak"`), 0 sinon.

**Exercice 0.7.7 (N3)** Écrire `void my_upcase(char *s)` qui met toutes les lettres minuscules en majuscules, sur place. Indice : une lettre minuscule et sa majuscule diffèrent de 32 dans la table ASCII.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-07)

## Fil rouge : vérifier la longueur

Un bon crackme rejette d'emblée une clé de mauvaise taille. On ajoute cette garde en tête de `verifier`, avec le `my_strlen` qu'on vient d'écrire :

```c
int verifier(const char *cle)
{
    if (my_strlen(cle) != 8)
        return 0;
    for (int i = 0; i < 8; i++)
        if ((unsigned char)(cle[i] + i) != cible[i])
            return 0;
    return 1;
}
```

Sans cette garde, une clé plus courte que 8 ferait lire la fonction au-delà du `'\0'`, en mémoire quelconque : exactement le genre de défaut qu'on cherche, plus tard, à exploiter.

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-07)

## Suivi

- [ ] Chapitre 0.7 : lecture
- [ ] Chapitre 0.7 : exercices
- [ ] Chapitre 0.7 : fil rouge
