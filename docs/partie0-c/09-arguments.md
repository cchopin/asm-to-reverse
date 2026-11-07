# 0.9 Les arguments du programme

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

`char **argv` est un pointeur vers des pointeurs de caractères (chapitre 0.6) : un tableau de chaînes. On le parcourt comme un tableau.

```c
for (int i = 0; i < argc; i++)
    my_putstr(argv[i]);
```

## Un argument est toujours une chaîne

Même `42` arrive sous forme de chaîne (`'4'`, `'2'`, `'\0'`), pas comme le nombre 42. Pour l'utiliser comme nombre, il faut le convertir, en parcourant les chiffres et en accumulant : `resultat = resultat * 10 + chiffre`. C'est ce que fait `atoi` de la libc, et l'exercice 0.9.3 le réécrit (`my_getnbr`). Ce motif de multiplication par 10 est très reconnaissable en assembleur.

## Exercices

**Exercice 0.9.1 (N1)** Écrire un programme qui affiche ses arguments un par ligne, en commençant à `argv[1]` (sans le nom du programme). Tester avec `./prog un deux trois`.

**Exercice 0.9.2 (N1)** Écrire un programme qui affiche le nombre d'arguments reçus (hors nom du programme), c'est-à-dire `argc - 1`.

**Exercice 0.9.3 (N2)** Écrire `int my_getnbr(const char *s)` qui convertit une chaîne en entier. Ignorer d'éventuels espaces en tête, lire une suite de signes `+`/`-` (chaque `-` inverse le signe), puis les chiffres, et s'arrêter au premier caractère non numérique. Exemples : `"42"` donne 42, `"  -42abc"` donne -42, `"+-+-13"` donne 13.

!!! warning "Contraintes"
    `atoi` et les autres fonctions de conversion de la libc sont interdites.

**Exercice 0.9.4 (N2)** Écrire un programme qui additionne tous ses arguments (supposés numériques) avec `my_getnbr` et affiche la somme. `./somme 10 20 12` affiche 42.

**Exercice 0.9.5 (N3)** Écrire un premier mini-crackme : le programme attend un nombre en argument, et affiche `Gagne` s'il vaut 1337, `Perdu` sinon. C'est la structure exacte des premiers défis de la partie 2, vue cette fois côté source.

Solutions : [solutions/partie0.md](../solutions/partie0.md#chapitre-09)

## Fil rouge : le coffre lit la clé

On branche enfin `verifier` sur la ligne de commande. Le coffre devient utilisable : `./coffre0 <cle>`.

```c
int main(int argc, char **argv)
{
    if (argc != 2) {
        my_putstr("Usage : ./coffre0 <cle>\n");
        return 1;
    }
    if (verifier(argv[1]))
        my_putstr("Coffre ouvert\n");
    else
        my_putstr("Refuse\n");
    return 0;
}
```

`./coffre0 Clemence` affiche `Coffre ouvert`, tout le reste `Refuse`. Le crackme est fonctionnel. Il reste à le ranger proprement (chapitre 0.10) et à le compiler en projet (chapitre 0.12).

Solution de l'étape : [solutions/partie0.md](../solutions/partie0.md#fil-rouge-09)

## Suivi

- [ ] Chapitre 0.9 : lecture
- [ ] Chapitre 0.9 : exercices
- [ ] Chapitre 0.9 : fil rouge
