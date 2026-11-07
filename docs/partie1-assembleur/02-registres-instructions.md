# Chapitre 2 : registres et premières instructions

Objectif : lire une instruction, connaître les registres et les quelques instructions qui représentent à elles seules la majorité du code rencontré.

## 2.1 Les registres

Le processeur ne calcule pas directement en mémoire : il utilise des **registres**, de petites zones de stockage très rapides intégrées au processeur. En x86-64, on dispose de 16 registres généraux de 64 bits :

| Registre | Rôle habituel (convention, pas obligation) |
|---|---|
| `rax` | valeur de retour d'une fonction, calculs |
| `rbx` | usage général (conservé entre les appels) |
| `rcx` | 4e argument, compteur |
| `rdx` | 3e argument, calculs (division) |
| `rsi` | 2e argument, source |
| `rdi` | 1er argument, destination |
| `rbp` | base de la pile de la fonction (chapitre 5) |
| `rsp` | sommet de la pile (chapitre 5) |
| `r8` à `r15` | registres supplémentaires (`r8` et `r9` = 5e et 6e arguments) |

S'y ajoutent deux registres spéciaux :

- `rip` : l'adresse de la **prochaine instruction** à exécuter. Il n'est pas modifié directement : ce sont les sauts et les appels qui le changent.
- `rflags` : les **drapeaux** (flags), qui conservent le résultat des comparaisons (chapitre 3).

### Les sous-registres

Chaque registre peut être utilisé par morceaux. C'est un héritage historique (le x86 date de 1978 et fonctionnait en 16 bits), omniprésent dans les listings :

```
 63                               31              15       7       0
 +--------------------------------+---------------+--------+-------+
 |                              rax (64 bits)                      |
 +--------------------------------+---------------+--------+-------+
                                  |          eax (32 bits)         |
                                  +---------------+--------+-------+
                                                  |    ax (16)     |
                                                  +--------+-------+
                                                  |   ah   |  al   |
                                                  +--------+-------+
```

| 64 bits | 32 bits | 16 bits | 8 bits (bas) |
|---|---|---|---|
| `rax` | `eax` | `ax` | `al` |
| `rbx` | `ebx` | `bx` | `bl` |
| `rcx` | `ecx` | `cx` | `cl` |
| `rdx` | `edx` | `dx` | `dl` |
| `rsi` | `esi` | `si` | `sil` |
| `rdi` | `edi` | `di` | `dil` |
| `r8` | `r8d` | `r8w` | `r8b` |

C'est un indice précieux sur les types : si le code utilise `eax`, la variable fait 32 bits (un `int`) ; s'il utilise `al`, c'est un `char` ; s'il utilise `rax`, c'est un `long` ou un pointeur.

Règle à connaître : **écrire dans un registre 32 bits met à zéro les 32 bits de poids fort**. `mov eax, 5` donne `rax = 5`. Ce n'est pas le cas pour les registres 8 et 16 bits.

## 2.2 Lire une instruction

En syntaxe **Intel** (celle de Ghidra, IDA, x64dbg et de ce cours) :

```
mnémonique  destination, source
mov         eax,         5         ; eax = 5
add         eax,         ebx       ; eax = eax + ebx
```

La destination est **à gauche**, et reçoit le résultat.

Attention : une autre syntaxe, **AT&T**, est utilisée par défaut par `objdump` et `gdb`. L'ordre y est inversé et tout est préfixé : `mov $5, %eax`. Pour obtenir la syntaxe Intel :

```bash
objdump -d -M intel programme
# dans gdb :
set disassembly-flavor intel
```

Dans le conteneur du cours, GDB est déjà configuré en syntaxe Intel.

### Les trois types d'opérandes

1. **Immédiat** : une constante. `mov eax, 0x2a`
2. **Registre** : `mov eax, ebx`
3. **Mémoire** : entre crochets, une adresse dont on lit ou écrit le contenu. `mov eax, DWORD PTR [rbp-0x8]` signifie "lire 4 octets à l'adresse `rbp - 8` et les placer dans eax".

`DWORD PTR` indique la taille lue. En C, `[adresse]` correspondrait à `*adresse` : un déréférencement de pointeur.

Une instruction ne peut pas avoir **deux** opérandes mémoire. C'est pourquoi le compilateur passe sans cesse par des registres : pour copier une variable dans une autre, il exécute `mov eax, [a]` puis `mov [b], eax`.

### Lire un listing objdump

`objdump -d -M intel` affiche le code d'un programme sous forme de **listing**. Chaque ligne a trois colonnes :

```
   4:   8d 44 be f9          lea    eax,[rsi+rdi*4-0x7]
   |          |                        |
 adresse    octets de l'instruction   instruction en assembleur
```

- l'**adresse** (à gauche) repère l'instruction ;
- les **octets** (au centre) sont l'instruction telle qu'elle est encodée dans le fichier : c'est le code machine réel. Ici, l'instruction `lea ...` tient sur 4 octets. Cette colonne est essentielle pour le patching (chapitre 4 de la partie 3) et les shellcodes (chapitre 3 de la partie 4) ;
- l'**instruction** (à droite) est la forme lisible, produite par `objdump`.

Une différence déroute souvent les débutants. Dans un **fichier objet** (`.o`, pas encore lié), les adresses partent de 0, car la position définitive n'est pas encore fixée :

```
0000000000000000 <calcul>:
   0:   f3 0f 1e fa          endbr64
   4:   8d 44 be f9          lea    eax,[rsi+rdi*4-0x7]
   8:   c3                   ret
```

Dans l'**exécutable** final (après édition de liens), la même fonction reçoit une adresse réelle, par exemple `0x401106`, et toutes les lignes sont décalées d'autant :

```
0000000000401106 <calcul>:
  401106:   f3 0f 1e fa          endbr64
  40110a:   8d 44 be f9          lea    eax,[rsi+rdi*4-0x7]
  40110e:   c3                   ret
```

Le code est identique ; seules les adresses changent. Pour alléger un listing quand les octets ne sont pas utiles, l'option `--no-show-raw-insn` masque la colonne du centre.

## 2.3 Les instructions essentielles

### mov : copier

```
mov eax, 10                    ; eax = 10
mov eax, ebx                   ; eax = ebx
mov eax, DWORD PTR [rbp-0x4]   ; eax = variable locale
mov DWORD PTR [rbp-0x4], 0x0   ; variable locale = 0
```

`mov` est de loin l'instruction la plus fréquente. Elle ne calcule rien, elle déplace.

### add, sub : additionner, soustraire

```
add eax, 3          ; eax = eax + 3
sub eax, ebx        ; eax = eax - ebx
add DWORD PTR [rbp-0x4], 0x1   ; variable++ (directement en mémoire)
```

### inc, dec, neg

```
inc eax             ; eax++
dec eax             ; eax--
neg eax             ; eax = -eax
```

### imul : multiplier

```
imul eax, ebx         ; eax = eax * ebx
imul eax, ebx, 12     ; eax = ebx * 12
```

(`mul` existe aussi, pour le non signé, sous une forme plus complexe qui utilise `rdx:rax`. Elle est moins fréquente.)

### div, idiv : diviser

C'est l'instruction la moins intuitive. `idiv ecx` divise le nombre 64 bits `edx:eax` par `ecx` : le **quotient** va dans `eax`, le **reste** dans `edx`. On trouve donc souvent `cdq` juste avant (qui prépare `edx` à partir du signe de `eax`).

```
mov eax, DWORD PTR [rbp-0x4]   ; eax = x
cdq                            ; étend le signe dans edx
idiv ecx                       ; eax = x / ecx, edx = x % ecx
```

Indice : si le code utilise `edx` après une division, il s'agit d'un **modulo** (`%`).

### Les opérations logiques

```
and eax, 0xff       ; eax = eax & 0xff (conserve l'octet de poids faible)
or  eax, 0x20       ; eax = eax | 0x20 (passe une lettre en minuscule)
xor eax, ebx        ; eax = eax ^ ebx
not eax             ; eax = ~eax
shl eax, 3          ; eax = eax << 3 (multiplie par 8)
shr eax, 1          ; eax = eax >> 1, non signé
sar eax, 1          ; eax = eax >> 1, signé (conserve le signe)
rol eax, 3          ; rotation à gauche (les bits sortants reviennent à droite)
```

### L'idiome xor reg, reg

```
xor eax, eax        ; eax = 0
```

Cette instruction apparaît des milliers de fois. Un XOR d'un registre avec lui-même donne toujours 0 ; le compilateur la préfère à `mov eax, 0` car elle est plus courte. **Ce n'est pas du chiffrement**, seulement une mise à zéro. En revanche, un `xor` entre deux valeurs **différentes**, dans une boucle, signale très souvent du chiffrement.

### lea : calculer une adresse (et pas seulement)

`lea` (Load Effective Address) calcule l'expression entre crochets **sans lire la mémoire**.

```
lea rax, [rbp-0x20]        ; rax = rbp - 0x20 (adresse d'une variable locale, comme &tab en C)
lea eax, [rdi+rsi*4-0x7]   ; eax = rdi + rsi*4 - 7 (un simple calcul)
```

Comparaison :

```
mov rax, [rbp-0x20]   ; rax = CONTENU de la mémoire à rbp-0x20
lea rax, [rbp-0x20]   ; rax = l'ADRESSE rbp-0x20 elle-même
```

Le compilateur détourne volontiers `lea` pour effectuer des calculs, car elle réalise une addition et une multiplication (par 1, 2, 4 ou 8) en une seule instruction.

### nop et endbr64

- `nop` ne fait rien ; il sert à aligner le code et peut être ignoré.
- `endbr64` en début de fonction est une protection (Intel CET). Pour l'analyse, il équivaut à un `nop`.

## 2.4 Exemple : du C à l'assembleur

Fichier `calcul.c` :

```c
int calcul(int a, int b)
{
    return a * 4 + b - 7;
}
```

Compilation sans optimisation et désassemblage :

```bash
gcc -O0 -c calcul.c -o calcul.o
objdump -d -M intel --no-show-raw-insn calcul.o
```

Résultat (gcc 13) :

```
push   rbp
mov    rbp,rsp
mov    DWORD PTR [rbp-0x4],edi     ; a est rangé dans une variable locale
mov    DWORD PTR [rbp-0x8],esi     ; b aussi
mov    eax,DWORD PTR [rbp-0x4]     ; eax = a
lea    edx,[rax*4+0x0]             ; edx = a * 4
mov    eax,DWORD PTR [rbp-0x8]     ; eax = b
add    eax,edx                     ; eax = b + a*4
sub    eax,0x7                     ; eax = b + a*4 - 7
pop    rbp
ret                                ; la fonction renvoie eax
```

Les instructions `push rbp`, `mov rbp, rsp` et `pop rbp` seront expliquées au chapitre 5. À observer ici :

- le premier argument `a` arrive dans `edi`, le second `b` dans `esi` ;
- le résultat est renvoyé dans `eax` ;
- sans optimisation, le compilateur range tout dans la pile puis relit : c'est verbeux mais très lisible.

Avec optimisation :

```bash
gcc -O2 -c calcul.c -o calcul.o && objdump -d -M intel --no-show-raw-insn calcul.o
```

```
lea    eax,[rsi+rdi*4-0x7]
ret
```

Une seule instruction : le même calcul, condensé grâce à `lea`. En CTF, les binaires sont souvent compilés en `-O0` ou `-O1` ; les malwares, souvent avec optimisation. Les deux cas seront travaillés.

### Les options de compilation utiles

Les commandes de ce cours emploient quelques options de `gcc`. Les voici regroupées une fois pour toutes :

| Option | Effet | Où elle compte |
|---|---|---|
| `-O0` | aucune optimisation : code verbeux mais très lisible | la plupart des exemples et des CTF faciles |
| `-O1`, `-O2` | optimisations croissantes : code plus court et plus astucieux | chapitre 7, et les malwares (souvent `-O2`) |
| `-c` | compiler sans faire l'édition de liens : produit un fichier objet `.o` | section 2.4 |
| `-no-pie` | exécutable chargé à une adresse fixe (pas de PIE) : adresses stables, plus simples à lire | pour obtenir des listings aux adresses constantes |
| `-fno-stack-protector` | retire le canari de pile (chapitre 5) | pour des listings épurés |
| `-s` | retire les symboles : le binaire devient *strippé* (chapitre 1 de la partie 3) | pour simuler un vrai échantillon |
| `-m32` | compiler en x86 32 bits | chapitre 7 |
| `-g` | ajouter les informations de débogage | pour confort dans GDB (chapitres 10, 15) |

Un exemple typique de compilation « propre » pour la lecture : `gcc -O0 -no-pie -fno-stack-protector prog.c -o prog`.

## 2.5 Méthode de lecture d'un bloc d'instructions

Un listing ne se lit pas "de tête". La méthode consiste à **suivre les valeurs** par écrit (papier ou fichier texte) :

1. Noter à côté de chaque instruction son effet en pseudo-C.
2. Nommer les emplacements mémoire dès que leur rôle est compris (`[rbp-0x4]` devient `a`).
3. Remplacer progressivement les registres par des expressions.

```
mov eax, [rbp-0x4]   ; eax = a
lea edx, [rax*4]     ; edx = 4a
mov eax, [rbp-0x8]   ; eax = b
add eax, edx         ; eax = b + 4a
sub eax, 7           ; eax = b + 4a - 7    => return 4*a + b - 7
```

C'est exactement le travail d'un décompilateur. Le faire à la main quelques dizaines de fois est ce qui rend autonome.

## 2.6 Observer les instructions dans GDB

Suivre les valeurs sur papier est indispensable, mais il est tout aussi utile de **vérifier** ce que fait réellement le processeur. Le débogueur GDB permet d'exécuter un programme instruction par instruction et d'afficher les registres après chacune. Il sera étudié en détail au chapitre 3 de la partie 3 ; quatre commandes suffisent pour l'instant.

Le fichier [exemples/observe.asm](../exemples/observe.asm) contient quelques instructions de ce chapitre, sans aucun code C autour. Il est écrit pour l'assembleur NASM, qui utilise la syntaxe Intel (l'écriture de programmes en assembleur est l'objet de la partie 4 ; ici, il suffit de l'assembler). Dans le conteneur :

```bash
cd /cours/exemples
nasm -f elf64 -g observe.asm -o observe.o   # assemblage : produit un fichier objet
ld observe.o -o observe                     # édition de liens : produit l'exécutable
gdb ./observe
```

Dans GDB :

| Commande | Effet |
|---|---|
| `starti` | Lance le programme et l'arrête **avant** sa première instruction |
| `si` | Exécute une seule instruction (*step instruction*) ; `si 5` en exécute cinq |
| `p $eax` | Affiche la valeur d'un registre en décimal ; `p/x $rax` l'affiche en hexadécimal |
| `p $eflags` | Affiche les flags actuellement à 1 (voir chapitre 3) |

Avec l'extension GEF installée dans le conteneur, chaque arrêt affiche en plus un grand écran récapitulatif : registres, pile, prochaines instructions. Ses zones sont décrites au chapitre 3 de la partie 3 ; pour l'instant, seules les commandes `p` sont nécessaires.

Session commentée (les lignes commençant par `$` sont les réponses de GDB) :

```
(gdb) starti
(gdb) si 5            # mov, mov, add, imul, sub
(gdb) p $eax
$1 = 40               # (5 + 4) * 5 - 5
(gdb) p $ebx
$2 = 5
(gdb) si              # lea ecx, [rax+rax*2]
(gdb) p $ecx
$3 = 120              # 40 * 3, et eax vaut toujours 40
(gdb) si 2            # xor edx, edx puis cmp eax, 40
(gdb) p $eflags
$4 = [ PF ZF IF ]     # ZF = 1 : la différence eax - 40 est nulle
(gdb) si              # cmp eax, 50
(gdb) p $eflags
$5 = [ CF PF SF IF ]  # SF = 1 (résultat négatif) et CF = 1 (40 < 50 en non signé)
(gdb) si              # mov rax, -1
(gdb) p/x $rax
$6 = 0xffffffffffffffff
(gdb) si              # mov al, 0x41
(gdb) p/x $rax
$7 = 0xffffffffffffff41
(gdb) si              # mov eax, 1
(gdb) p/x $rax
$8 = 0x1
```

Les trois dernières étapes illustrent une règle de la section 2.1 souvent oubliée : écrire dans `al` ne modifie que l'octet de poids faible, alors qu'écrire dans `eax` **remet à zéro** les 32 bits hauts de `rax`. Les flags `PF` et `IF` peuvent être ignorés : `PF` (parité) ne sert presque jamais en reverse, et `IF` (interruptions autorisées) est toujours à 1 pour un programme normal.

La commande `quit` (ou `q`) quitte GDB. La démarche est toujours la même : prévoir la valeur sur papier, exécuter, comparer. Un écart signale une instruction mal comprise.

## Exercices

**Exercice 2.1** Après ces instructions, que valent `eax` et `ebx` ?

```
mov eax, 7
mov ebx, eax
add eax, 3
imul eax, ebx
sub eax, 0x10
```

**Exercice 2.2** Traduire en C (une seule ligne) :

```
mov eax, DWORD PTR [rbp-0x8]
add eax, eax
add eax, DWORD PTR [rbp-0x8]
shl eax, 2
```

Indice : suivre la valeur. Que fait `add eax, eax` ?

**Exercice 2.3** Que contient `eax` à la fin ? Et `edx` ?

```
mov eax, 47
cdq
mov ecx, 5
idiv ecx
```

**Exercice 2.4** Que fait cette instruction, et quel est son équivalent avec `mov`, `shl` et `add` ?

```
lea eax, [rdi+rdi*2]
```

**Exercice 2.5** (la partie `f3` en `-O2` est un bonus, voir chapitre 7) Écrire ces trois fonctions en C, les compiler en `-O0` puis en `-O2`, et comparer. Noter à chaque fois ce que fait le compilateur.

```c
int f1(int x) { return x * 8; }
int f2(int x) { return x * 9; }
int f3(int x) { return x / 2; }
```

Pour `f3` en `-O2`, une instruction `shr eax, 0x1f` inattendue apparaît. Chercher à l'expliquer (indice : en C, la division entière d'un nombre négatif arrondit vers zéro, alors qu'un décalage arrondit vers moins l'infini).

**Exercice 2.6** Après avoir résolu les exercices 2.1 et 2.3 sur papier, recopier `observe.asm` sous un autre nom, remplacer les instructions de `_start` (en gardant les trois dernières lignes, qui terminent le programme) par celles de l'exercice 2.1, puis vérifier les réponses dans GDB. Faire de même avec l'exercice 2.3 (en NASM, `cdq` et `idiv ecx` s'écrivent tels quels).

## Exercices de lecture

Ces exercices portent sur du vrai désassemblage. Pour reproduire les listings : écrire la fonction dans `ex.c`, puis `gcc -O0 -fno-stack-protector -no-pie -c ex.c` et `objdump -d ex.o`. Les adresses peuvent varier d'une machine à l'autre, la logique non.

Fonction `calc`, compilée sans optimisation :

```text
<calc>:
  push   %rbp
  mov    %rsp,%rbp
  mov    %edi,-0x4(%rbp)
  mov    %esi,-0x8(%rbp)
  mov    -0x4(%rbp),%edx
  mov    %edx,%eax
  add    %eax,%eax
  add    %eax,%edx
  mov    -0x8(%rbp),%eax
  add    %edx,%eax
  sub    $0x7,%eax
  pop    %rbp
  ret
```

**Exercice 2.L1 (N1)** Dans quels registres arrivent le premier et le deuxième argument ? À quels emplacements de la pile sont-ils recopiés ?

**Exercice 2.L2 (N2)** Les instructions `mov %edx,%eax` puis `add %eax,%eax` puis `add %eax,%edx` calculent une valeur à partir du premier argument. Laquelle, et pourquoi (que fait `add %eax,%eax` sur `eax`) ?

**Exercice 2.L3 (N3)** Reconstituer l'expression renvoyée par `calc` en fonction de ses deux arguments `a` et `b`.

Solutions : [solutions/partie1.md](../solutions/partie1.md#lecture-02)

## Fil rouge : premières lignes de verifier

Dans [mystere_O0.txt](../fil-rouge/partie1/mystere_O0.txt) :

1. Les trois premières instructions après `endbr64` sont `push rbp`, `mov rbp,rsp`, puis `mov QWORD PTR [rbp-0x18],rdi`. D'après ce chapitre, que contient `rdi` à l'entrée d'une fonction ? Quelle est la taille de ce qui est rangé en `[rbp-0x18]` ? À quel type C cela pourrait-il correspondre ?
2. À l'adresse `401162` se trouve `mov DWORD PTR [rbp-0x4],0x0`. Quelle est la taille de cette variable ? Lui donner un nom provisoire.
3. À l'adresse `40116b`, `add DWORD PTR [rbp-0x4],0x1`. Quel est l'équivalent en C ?
4. Repérer les instructions entre `4011d2` et `4011e8` : un `sub eax,0x30` puis un `add DWORD PTR [rbp-0x8],eax`. En se rappelant le chapitre 1 (que vaut `0x30` en ASCII ?), proposer une interprétation.
5. Les dernières instructions avant `pop rbp` sont des `mov eax, 0x0` ou `mov eax, 0x1`. Que renvoie probablement la fonction ?

Compléter `notes_mystere.md`.

Solutions : [solutions/partie1.md](../solutions/partie1.md#chapitre-2)

## Suivi

- [ ] Partie 2, chapitre 2 : lecture
- [ ] Partie 2, chapitre 2 : exercices
- [ ] Partie 2, chapitre 2 : fil rouge
