# Résolution automatique : angr et z3

!!! info "Chapitre d'approfondissement"
    Approfondissement de la partie 3. Jusqu'ici, on résolvait les crackmes à la main. Parfois, il est plus rapide de **laisser une machine chercher** : un solveur de contraintes (`z3`) ou un moteur d'exécution symbolique (`angr`) trouve l'entrée qui mène au succès. Ce chapitre présente les deux, et surtout quand les employer.

!!! note "Note sur l'environnement"
    Les exemples `z3` ci-dessous sont réels et testés. `z3` et `angr` sont tous deux fournis dans l'image du laboratoire (`reverse-lab`, voir l'installation sur Mac) : vous pouvez y rejouer directement les scripts z3 comme les scripts angr, sans rien installer de plus. Les scripts sont dans `angr-z3/`.

## 12.1 L'idée : l'exécution symbolique

Plutôt que d'essayer des entrées concrètes une par une, on traite l'entrée comme une **variable symbolique** (« un inconnu »), et on exprime la condition de victoire comme une équation. Un solveur cherche alors une valeur de l'inconnu qui satisfait l'équation. On ne devine plus : on décrit le problème et on laisse l'outil le résoudre.

Deux niveaux d'outillage :

- **z3** : on lit le code, on traduit la vérification en contraintes mathématiques, z3 donne la solution. C'est à nous de modéliser.
- **angr** : on donne le binaire, les adresses « gagné » et « perdu », et angr explore tout seul les chemins en entrée symbolique jusqu'à atteindre la victoire. Plus automatique, mais plus lourd.

## 12.2 z3 sur un système de contraintes

Reprenons le crackme `crack05` du CTF maison : quatre chiffres soumis à des conditions. On traduit chaque condition en contrainte et z3 fait le reste :

```python
from z3 import Int, Solver, sat
d = [Int('d%d' % i) for i in range(4)]
s = Solver()
for x in d: s.add(x >= 0, x <= 9)          # chaque chiffre
s.add(d[0] + d[1] + d[2] + d[3] == 20)
s.add(d[0] == d[3] + 1)
s.add(d[1] == d[2] * 2)
s.add(d[0] > d[1])
s.check()                                   # sat
m = s.model()                               # -> 9218
```

z3 répond `sat` (satisfiable) et donne `9218`, la seule solution. On aurait pu la chercher à la main, mais dès que les contraintes se multiplient, le solveur gagne.

## 12.3 z3 avec des bitvectors

Les opérations de bas niveau (XOR, rotations, débordements sur 8 ou 32 bits) se modélisent avec des **bitvectors**, des entiers de largeur fixe qui se comportent exactement comme des registres. Reprenons la VM du défi `machine` du capstone, où chaque caractère subit `rol8((c ^ 0x2A) + 5, 3)` et doit égaler une cible :

```python
from z3 import BitVec, Solver, RotateLeft
cible = [0xEA,0xA2,0x0B,0xA2,0xEA,0xF2,0xA2,0x19,0xE8]
cs = [BitVec('c%d' % i, 8) for i in range(len(cible))]
s = Solver()
for i, c in enumerate(cs):
    v = RotateLeft((c ^ 0x2A) + 5, 3)
    s.add(v == cible[i])
    s.add(c >= 0x20, c <= 0x7e)   # imprimable
s.check()                          # -> reverse42
```

z3 retrouve `reverse42`. L'immense avantage : on n'a pas eu à **inverser** la transformation (ce qui, au chapitre des solutions du capstone, demandait de calculer `ror`, soustraire, re-XOR). On décrit le sens direct, z3 inverse pour nous. Pour des transformations difficiles à inverser à la main, c'est décisif.

## 12.4 angr : explorer automatiquement

Quand on ne veut même pas lire le code, `angr` exécute le binaire **symboliquement** et explore les chemins jusqu'à une adresse cible :

```python
import angr
proj = angr.Project("crackme", auto_load_libs=False)
simgr = proj.factory.simulation_manager(proj.factory.full_init_state())
simgr.explore(find=0x401180,   # adresse du bloc "gagné"
              avoid=0x4011a0)  # adresse du bloc "perdu"
if simgr.found:
    print(simgr.found[0].posix.dumps(0))   # l'entrée (stdin) qui gagne
```

On relève dans le désassemblage l'adresse où le programme affiche le succès (`find`) et celle de l'échec (`avoid`). angr trouve une entrée qui atteint `find` en évitant `avoid`, puis `posix.dumps(0)` donne ce qu'il aurait fallu taper. C'est souvent magique sur les petits crackmes, au prix d'une installation plus lourde et d'un temps de calcul qui explose si les chemins sont trop nombreux.

## 12.5 z3 ou angr ?

| Situation | Outil |
|---|---|
| On lit le code, les contraintes sont claires | z3 (on modélise, c'est rapide et fiable) |
| Transformation pénible à inverser à la main | z3 (il inverse pour nous) |
| On ne veut pas lire le code, petit binaire | angr (exploration automatique) |
| Beaucoup de chemins, boucles longues, I/O complexes | ni l'un ni l'autre tout seul (explosion) : réduire d'abord le périmètre |

En pratique : z3 quand on comprend la vérification et qu'on sait l'écrire en équations ; angr quand on préfère une boîte plus noire et que le binaire reste petit. Les deux se combinent (angr utilise z3 en interne).

!!! warning "Ce ne sont pas des boutons magiques"
    L'exécution symbolique souffre de l'explosion des chemins : une boucle sur une entrée longue, du code chiffré, des appels système non modélisés peuvent bloquer angr. Ces outils complètent l'analyse manuelle, ils ne la remplacent pas : il faut souvent cibler une portion précise du binaire.

## Exercices gradués

Scripts dans `angr-z3/`. Les exercices z3 comme ceux avec angr sont jouables directement dans l'image du laboratoire (`reverse-lab`), où les deux outils sont fournis.

**Exercice 12.1 (niveau 1)** Lancer `solve_crack05.py` et vérifier qu'il donne `9218`. Modifier une contrainte (par exemple la somme à 21) et observer si z3 répond encore `sat`.

**Exercice 12.2 (niveau 2)** Lancer `solve_vm.py` et retrouver `reverse42`. Expliquer pourquoi on n'a pas eu besoin d'inverser `rol8` et le `+5` à la main, contrairement à la solution « manuelle » du capstone.

**Exercice 12.3 (niveau 2)** Modéliser avec z3 le fil rouge `coffre0` de la partie 1 : trouver une clé telle que `cle[i] + i == cible[i]` pour la cible `43 6d 67 70 69 73 69 6c`. Vérifier qu'on retrouve `Clemence`.

**Exercice 12.4 (niveau 3)** Écrire un check volontairement pénible à inverser (plusieurs XOR, additions et rotations enchaînés sur chaque octet), calculer la cible pour un mot choisi, puis laisser z3 retrouver le mot à partir de la seule cible et des contraintes.

**Exercice 12.5 (niveau 3)** Installer angr et résoudre un petit crackme avec `solve_angr.py` : relever les adresses `find` (succès) et `avoid` (échec) dans le désassemblage, lancer l'exploration, récupérer l'entrée gagnante. Comparer le temps et l'effort avec la résolution manuelle.

**Exercice 12.6 (niveau 2)** Pour trois crackmes du cours (par exemple `crack01` comparaison directe, `crack05` contraintes, `machine` VM), dire pour chacun si z3, angr ou l'analyse manuelle est le plus adapté, et pourquoi.

Solutions : [solutions/partie2.md](../solutions/partie2.md#angr-z3)

## Suivi

- [ ] Partie 3, chapitre angr/z3 : lecture
- [ ] Partie 3, chapitre angr/z3 : z3 pratiqué (crack05, VM)
- [ ] Partie 3, chapitre angr/z3 : angr essayé (dans le laboratoire)
