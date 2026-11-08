# Chapitre 9 : Les arguments du programme

Presque tous les crackmes attendent leur mot de passe sur la ligne de commande : `./crackme MONCODE`. Savoir lire ces arguments est donc indispensable. Ce chapitre les présente et branche enfin le fil rouge sur une vraie clé.

## argc et argv

`main` peut recevoir deux paramètres :

```c
int main(int argc, char **argv)
{
    // argc : nombre d'arguments, le nom du programme compris
    // argv : tableau de chaînes ; argv[0] est le nom du programme
    return 0;
}
```

Si on lance `./prog bonjour 42`, alors :

- `argc` vaut 3 ;
- `argv[0]` est `"./prog"` ;
- `argv[1]` est `"bonjour"` ;
- `argv[2]` est `"42"`.

`char **argv` est un pointeur vers des pointeurs de caractères (chapitre 6) : un tableau de chaînes. On le parcourt comme un tableau.

```c
for (int i = 0; i < argc; i++)
    my_putstr(argv[i]);
```

## Un argument est toujours une chaîne

Même `42` arrive sous forme de chaîne (`'4'`, `'2'`, `'\0'`), pas comme le nombre 42. Pour l'utiliser comme nombre, il faut le convertir, en parcourant les chiffres et en accumulant : `resultat = resultat * 10 + chiffre`. C'est ce que fait `atoi` de la libc, et l'exercice 9.3 le réécrit (`my_getnbr`). Ce motif de multiplication par 10 est très reconnaissable en assembleur.

## Exercices

Un fichier `.c` par exercice, compilé avec `gcc -Wall -Wextra` et testé en ligne de commande. Solutions à part, dans [solutions/partie0.md](../solutions/partie0.md#chapitre-09).

### Exercice 9.1 - afficher les arguments (N1)

- **Fichier à rendre :** `mes_arguments.c`
- **Prototype :** programme complet (`int main(int argc, char **argv)`)
- **Fonctions autorisées :** `my_putstr`, `my_putchar`

Afficher les arguments un par ligne, en commençant à `argv[1]` (sans le nom du programme).

Exemple d'exécution attendu :

```text
$ ./mes_arguments un deux trois
un
deux
trois
```

### Exercice 9.2 - compter les arguments (N1)

- **Fichier à rendre :** `compter_arguments.c`
- **Prototype :** programme complet
- **Fonctions autorisées :** `printf`

Afficher le nombre d'arguments reçus hors nom du programme, c'est-à-dire `argc - 1`.

Exemple d'exécution attendu :

```text
$ ./compter_arguments un deux trois
3
```

### Exercice 9.3 - my_getnbr (N2)

- **Fichier à rendre :** `my_getnbr.c`
- **Prototype :** `int my_getnbr(const char *s);`
- **Fonctions autorisées :** aucune (`atoi` et les conversions de la libc sont interdites)

Convertir une chaîne en entier. Ignorer d'éventuels espaces en tête, lire une suite de signes `+`/`-` (chaque `-` inverse le signe), puis les chiffres, et s'arrêter au premier caractère non numérique.

Exemple d'exécution attendu (appels sur `"42"`, `"  -42abc"`, `"+-+-13"`) :

```text
42 -42 13
```

### Exercice 9.4 - somme des arguments (N2)

- **Fichier à rendre :** `somme_args.c`
- **Prototype :** programme complet
- **Fonctions autorisées :** `my_getnbr` (votre fonction du 9.3), `printf`

Additionner tous les arguments (supposés numériques) et afficher la somme.

Exemple d'exécution attendu :

```text
$ ./somme_args 10 20 12
42
```

### Exercice 9.5 - premier mini-crackme (N3)

- **Fichier à rendre :** `crackme0.c`
- **Prototype :** programme complet
- **Fonctions autorisées :** `my_getnbr`, `printf`

Le programme attend un nombre en argument et affiche `Gagne` s'il vaut 1337, `Perdu` sinon. C'est la structure exacte des premiers défis de la partie 3, vue cette fois côté source.

Exemple d'exécution attendu :

```text
$ ./crackme0 1337
Gagne
$ ./crackme0 42
Perdu
```

## Fil rouge : le coffre lit la clé

On branche enfin `verifier` sur la ligne de commande. Le coffre devient utilisable : `./coffre0 <cle>`.

À vous de compléter `coffre0` :

- **Fichier à rendre :** `coffre0.c`
- **Prototype :** programme complet (`int main(int argc, char **argv)`)
- **Fonctions autorisées :** `verifier` (des chapitres précédents), `my_putstr`

Consignes :

- Si le programme ne reçoit pas exactement un argument (`argc != 2`), afficher `Usage : ./coffre0 <cle>` et renvoyer 1.
- Sinon, appeler `verifier(argv[1])` et afficher `Coffre ouvert` si elle renvoie 1, `Refuse` sinon.

Le crackme est fonctionnel : `./coffre0 Clemence` affiche `Coffre ouvert`, tout le reste `Refuse`. Il reste à le ranger proprement (chapitre 10) et à le compiler en projet (chapitre 12).

Exemple d'exécution attendu :

```text
$ ./coffre0 Clemence
Coffre ouvert
$ ./coffre0 mauvais
Refuse
```

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-09)

## Suivi

- [ ] Partie 1, chapitre 9 : lecture
- [ ] Partie 1, chapitre 9 : exercices
- [ ] Partie 1, chapitre 9 : fil rouge
