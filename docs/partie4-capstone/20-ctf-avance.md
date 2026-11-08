# Les défis CTF avancés

Trois crackmes, de difficulté croissante, à vaincre dans l'ordre. Chacun combine plusieurs techniques vues dans les parties précédentes. Les sources sont fournies pour pouvoir compiler les binaires soi-même, mais **le jeu consiste à travailler sur le binaire**, pas à lire le `.c`.

## Préparer les binaires

```bash
cd capstone/ctf
make            # produit serrure, machine, oignon
```

!!! note "Règle du jeu"
    On ne lit pas les fichiers `.c` avant d'avoir trouvé le flag. On les garde pour vérifier son analyse après coup. Chaque défi délivre un flag de la forme `FLAG{...}` quand on lui donne la bonne entrée.

## Défi 1 : serrure (keygen)

```bash
./serrure <nom> <serial-hex>
```

- **Binaire :** `serrure`
- **Fichier à rendre :** un keygen
- **À produire :** le flag `FLAG{serrure_XXXXXX}`

Le programme attend un nom et un serial hexadécimal. Pour un nom donné, un seul serial est accepté. Le flag est `FLAG{serrure_XXXXXX}` où `XXXXXX` est ce serial.

**Objectif** : retrouver l'algorithme qui transforme le nom en serial, puis écrire un *keygen* (en Python, ou en assembleur pour les plus ambitieux, sur le modèle du fil rouge de la partie 4) qui donne le serial de n'importe quel nom, à commencer par `Clemence`.

**Techniques en jeu** : lecture d'une fonction de hachage en assembleur (boucle, `xor`, rotation, constantes), reconnaissance d'un keygen (partie 3, chapitre 4 ; partie 4, fil rouge).

!!! tip "Indice"
    La fonction de dérivation tient en une boucle sur les caractères du nom. Repérer la constante initiale, l'opération par caractère, et le masque final. Rien n'est chiffré : tout est dans le calcul.

## Défi 2 : machine (machine virtuelle)

```bash
./machine <mot-de-passe>
```

- **Binaire :** `machine`
- **À produire :** le mot de passe accepté et le flag

Ce défi ne compare pas le mot de passe à une chaîne, et n'applique pas non plus un simple XOR. Il embarque une **petite machine virtuelle** : un tableau d'octets est interprété comme un programme qui transforme chaque caractère de l'entrée, et le résultat est comparé à une cible stockée.

**Objectif** : comprendre le jeu d'instructions de la VM (quelques opcodes), extraire le programme et la cible, puis inverser la transformation pour reconstruire le mot de passe.

**Techniques en jeu** : reconnaissance d'un interpréteur (boucle de dispatch sur un opcode), lecture de tableaux en mémoire (partie 2, chapitre 6), inversion d'opérations (`add`/`sub`, `xor` involutif, `rol`/`ror`). C'est le schéma le plus fréquent des CTF de reverse de niveau intermédiaire.

!!! tip "Indice"
    Chercher la boucle qui lit un octet d'opcode, puis une série de comparaisons (`cmp` avec de petites constantes) dirigeant vers quelques traitements différents : c'est la table de dispatch de la VM. Le même programme est rejoué pour chaque caractère.

## Défi 3 : oignon (multi-couches et anti-debug)

```bash
./oignon <mot-de-passe>
```

- **Binaire :** `oignon`
- **À produire :** le mot de passe reconstitué et le flag

Le plus retors. Il se protège d'un débogueur et ne stocke le mot de passe attendu nulle part en clair : celui-ci est **reconstruit à l'exécution** par combinaison de plusieurs tableaux.

**Objectif** : neutraliser l'anti-debug si l'on choisit la voie dynamique, repérer comment le mot de passe attendu est recomposé, et le reconstituer. La voie statique (recomposer soi-même sans lancer le binaire) est la plus propre.

**Techniques en jeu** : anti-debug `ptrace` (partie 3, chapitre 5 ; partie 4, section 1.6) et son contournement, XOR entre deux tableaux (partie 3, chapitre 4), lecture de `.data` avec `objdump -s`.

!!! tip "Indice"
    Si le programme affiche `Nope.` et s'arrête, c'est l'anti-debug : le contourner comme au chapitre 5 de la partie 3 (patch de l'appel, `LD_PRELOAD` d'un faux `ptrace`, ou forcer `rax` dans GDB). Pour la voie statique, relever les deux tableaux de `.data` et les combiner : l'opération qui les réunit est juste avant le `memcmp`.

## Après les flags

Une fois les trois flags obtenus, comparer sa démarche aux write-ups : [solutions/partie4.md](../solutions/partie4.md#defis-ctf). L'important n'est pas seulement d'avoir trouvé, mais d'avoir pris le chemin le plus court et le plus sûr.

## Suivi

- [ ] Défi 1 (serrure) : flag obtenu et keygen écrit
- [ ] Défi 2 (machine) : flag obtenu et VM inversée
- [ ] Défi 3 (oignon) : flag obtenu, anti-debug contourné
