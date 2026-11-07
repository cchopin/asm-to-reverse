# Challenge de synthèse : un outil Unix sans libc

Ce challenge clôt la partie 4. Il ne s'agit plus de petits exercices isolés, mais d'écrire un vrai petit outil, utile et autonome, puis de le relire comme le ferait un analyste. Il réunit tout ce que la partie 4 a introduit : appels système `read`/`write`, boucles, manipulation d'octets, et absence totale de libc.

## L'objectif

Écrire `filtre_majuscules`, un filtre Unix qui lit l'entrée standard, met en majuscules chaque lettre minuscule (`a`-`z` vers `A`-`Z`), laisse tout le reste intact, et écrit le résultat sur la sortie standard. Comme tout filtre, il doit fonctionner dans un tube :

```
$ printf 'Bonjour Clemence, reverse 2026!\n' | ./filtre_majuscules
BONJOUR CLEMENCE, REVERSE 2026!
$ ./filtre_majuscules < mon_fichier.txt > en_majuscules.txt
```

## Les contraintes

!!! note "Contraintes"
    - Aucune libc : uniquement les appels système `read`, `write` et `exit`. Édition de liens avec `ld`, pas `gcc`.
    - Traiter l'entrée par blocs, dans un tampon en section `.bss`, et boucler jusqu'à la fin de fichier (`read` renvoie `0`).
    - La transformation se fait sur place dans le tampon, avant le `write`.
    - Ne toucher qu'aux octets de `a` à `z`. Les chiffres, espaces, accents et ponctuations passent tels quels.

## La démarche

1. Reprendre la boucle de lecture du mini-`cat` (exercice 15.7) : `read(0, buf, 4096)`, puis tester le retour.
2. Avant le `write`, parcourir les octets lus un par un. Pour chaque octet entre `'a'` et `'z'`, soustraire `0x20` (l'écart constant entre minuscules et majuscules en ASCII, déjà vu en partie 1).
3. Écrire le bloc transformé, puis repartir lire le suivant.
4. Sur fin de fichier, `exit(0)`.

Le point clé est l'ordre des comparaisons : un octet est une minuscule si et seulement s'il est **à la fois** supérieur ou égal à `'a'` et inférieur ou égal à `'z'`. Un seul des deux tests ne suffit pas.

## La deuxième moitié : se relire

Un outil qui marche ne prouve pas qu'on l'a compris. Une fois le filtre fonctionnel :

1. Le désassembler avec `objdump -d -M intel filtre_majuscules`.
2. Repérer et annoter, dans le listing, chacune des trois phases : la lecture (`read`), la boucle de transformation (les deux `cmp` et le `sub`), puis l'écriture (`write`).
3. Vérifier qu'on retrouve bien `0x20` comme constante soustraite, et les bornes `0x61` (`'a'`) et `0x7a` (`'z'`) dans les comparaisons.

C'est exactement le mouvement du reverse : écrire du code, puis le reconnaître sous sa forme machine. Un analyste qui voit cette signature (deux `cmp` encadrant une plage de 26 valeurs suivis d'un `sub 0x20`) sait immédiatement qu'il regarde une conversion de casse, sans avoir besoin du code source.

## Pour aller plus loin

- Variante `rot13` : au lieu de soustraire `0x20`, décaler chaque lettre de 13 positions dans l'alphabet (en bouclant). C'est le chiffrement le plus simple qui soit, et un grand classique des CTF.
- Variante `compteur` : au lieu de transformer, compter les octets et afficher le total à la fin, comme un `wc -c` minimal.

Solutions : [solutions/partie3.md](../solutions/partie3.md#challenge)

## Suivi

- [ ] Challenge partie 4 : filtre majuscules écrit et testé
- [ ] Challenge partie 4 : filtre désassemblé et annoté
