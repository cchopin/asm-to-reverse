# Chapitre 1 : écrire de l'assembleur, premier programme et appels système

Les deux premières parties ont appris à **lire** l'assembleur produit par un compilateur. Cette troisième partie fait le chemin inverse : **écrire** de l'assembleur à la main, avec l'assembleur NASM. Ce détour est utile au reverse pour trois raisons :

- écrire une instruction fixe sa signification bien mieux que la lire ;
- les shellcodes (chapitre 3), une partie des malwares et les challenges de type *pwn* (exploitation) sont écrits ou assemblés à la main ;
- un programme écrit en assembleur ne ressemble pas à du code de gcc : il faut savoir le reconnaître.

Quand lire cette partie ? Les chapitres 1 et 2 (hors fil rouge) se lisent dès la fin de la partie 2 : ils ne demandent que les chapitres 2 à 6 de la partie 2. En revanche, **les fils rouges et le chapitre 3 supposent la partie 3** : le fil rouge du chapitre 1 reprend le keygen du coffre v3 (chapitre 4 de la partie 3), la section 1.6 s'appuie sur l'anti-debug (chapitre 5 de la partie 3), et le chapitre 3 mobilise GDB (chapitre 3 de la partie 3), le XOR et la lecture des octets (chapitre 4 de la partie 3) et les loaders de malware (chapitre 6 de la partie 3). L'idéal est donc de lire les chapitres 1 et 2 en parallèle de la partie 3, et de garder les fils rouges et le chapitre 3 pour la fin.

## 1.1 Structure d'un fichier NASM

Un fichier NASM (extension `.asm`) est découpé en **sections**, qui correspondent à celles de l'exécutable final (chapitre 6 de la partie 2) :

| Section | Contenu |
|---|---|
| `section .data` | données initialisées (chaînes, constantes) |
| `section .bss` | données non initialisées (tampons), mises à zéro au lancement |
| `section .rodata` | données en lecture seule |
| `section .text` | le code |

Les principales directives :

| Écriture | Signification |
|---|---|
| `message db "abc", 10` | réserve des octets avec une valeur initiale (`db` : 1 octet, `dw` : 2, `dd` : 4, `dq` : 8) |
| `tampon resb 64` | réserve 64 octets sans valeur (`resb`, `resw`, `resd`, `resq`), en `.bss` |
| `longueur equ $ - message` | constante calculée à l'assemblage : `$` est l'adresse courante, donc la différence est la taille de `message` |
| `etiquette:` | une étiquette : un nom donné à une adresse |
| `.boucle:` | une étiquette locale (rattachée à la dernière étiquette sans point) |
| `global _start` | rend le symbole visible pour l'éditeur de liens |
| `; texte` | un commentaire |

La syntaxe des instructions est la syntaxe Intel utilisée depuis le début du cours, avec deux différences de détail : la taille s'écrit `byte`, `word`, `dword`, `qword` sans `PTR` (par exemple `cmp byte [rsi], 0`), et `[rel message]` demande une adresse relative à `rip` (chapitre 6 de la partie 2), équivalente au `[rip+0x...]` des listings.

Un programme passe par deux étapes avant de pouvoir être lancé :

```mermaid
flowchart LR
    A["bonjour.asm<br/>(texte)"] -->|"nasm -f elf64"| B["bonjour.o<br/>(fichier objet)"]
    B -->|"ld"| C["bonjour<br/>(exécutable ELF)"]
    B -->|"gcc, si la libc est utilisée"| C
```

L'**assembleur** traduit chaque instruction en octets. L'**éditeur de liens** (`ld`) place les sections à leurs adresses définitives et fixe le point d'entrée. Sans libc, le point d'entrée s'appelle `_start` : il n'y a pas de `main`, et rien n'est exécuté avant la première instruction de `_start`.

## 1.2 Les appels système

Un programme ne peut pas accéder lui-même à l'écran, aux fichiers ou au réseau : seul le **noyau** (le cœur du système d'exploitation) le peut. Pour lui demander un service, le programme effectue un **appel système** (*syscall*).

Il faut bien distinguer deux notions jusqu'ici confondues :

- une **fonction de bibliothèque** comme `printf` ou `strcmp` est du code ordinaire, situé dans la libc et exécuté dans le programme ; c'est ce que montre `ltrace` ;
- un **appel système** comme `write` ou `ptrace` fait passer la main au noyau ; c'est ce que montre `strace`.

`printf` finit par appeler l'appel système `write`, mais `strcmp` n'en fait aucun. En assembleur, l'appel système passe par l'instruction `syscall`, avec une convention propre :

| Registre | Rôle |
|---|---|
| `rax` | **numéro** de l'appel système |
| `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9` | arguments 1 à 6 |
| `rax` (après) | valeur de retour ; une valeur négative entre -1 et -4095 signale une erreur (`-errno`) |

Deux différences avec la convention d'appel des fonctions (chapitre 5 de la partie 2) sont à retenir : le quatrième argument est dans **`r10`** et non dans `rcx`, et l'instruction `syscall` **écrase `rcx` et `r11`**. Les autres registres sont conservés.

Les numéros les plus fréquents en x86-64 :

| Numéro | Nom | Prototype simplifié |
|---|---|---|
| 0 | `read` | `read(fd, tampon, taille)` |
| 1 | `write` | `write(fd, tampon, taille)` |
| 2 | `open` | `open(chemin, drapeaux, mode)` |
| 3 | `close` | `close(fd)` |
| 9 | `mmap` | réserve de la mémoire (chapitre 3) |
| 10 | `mprotect` | change les droits d'une zone mémoire |
| 41, 42 | `socket`, `connect` | réseau |
| 57 | `fork` | crée un processus |
| 59 | `execve` | remplace le programme par un autre (chapitre 3) |
| 60 | `exit` | termine le programme |
| 101 | `ptrace` | débogage (chapitre 5 de la partie 3) |

La liste complète se trouve dans le conteneur :

```bash
grep -E '__NR_(read|write|exit|ptrace) ' /usr/include/x86_64-linux-gnu/asm/unistd_64.h
```

Les **descripteurs de fichiers** (`fd`) 0, 1 et 2 sont ouverts au lancement de tout programme : 0 est l'entrée standard (le clavier, ou ce qui arrive par `|` ou `<`), 1 la sortie standard (le terminal), 2 la sortie d'erreur. Les numéros diffèrent en 32 bits (`int 0x80`, `eax = 4` pour `write`) et sous Windows (où les programmes passent toujours par les DLL) : une table est donc toujours propre à un système et à une architecture.

## 1.3 Premier programme

[exemples/bonjour.asm](../exemples/bonjour.asm) :

```nasm
; bonjour.asm : afficher un message puis quitter, sans libc
section .data
    message db "Bonjour depuis l'assembleur", 10   ; 10 = '\n'
    longueur equ $ - message                       ; calculé par NASM

section .text
    global _start

_start:
    mov rax, 1          ; numéro de l'appel système write
    mov rdi, 1          ; 1er argument : descripteur 1 (sortie standard)
    mov rsi, message    ; 2e argument : adresse du texte
    mov rdx, longueur   ; 3e argument : nombre d'octets
    syscall

    mov rax, 60         ; numéro de l'appel système exit
    xor rdi, rdi        ; code de retour 0
    syscall
```

```bash
cd /cours/exemples
nasm -f elf64 bonjour.asm -o bonjour.o
ld bonjour.o -o bonjour
./bonjour
```

Le second appel système est indispensable : sans `exit`, le processeur continuerait à exécuter les octets situés après le dernier `syscall`, et le programme planterait (`Segmentation fault`).

`strace` confirme qu'il n'y a rien d'autre que les deux appels écrits :

```
$ strace ./bonjour
execve("./bonjour", ["./bonjour"], 0x7ffd... /* 20 vars */) = 0
write(1, "Bonjour depuis l'assembleur\n", 28Bonjour depuis l'assembleur
) = 28
exit(0)                                 = ?
```

À titre de comparaison, un `puts("Bonjour")` compilé avec gcc effectue une trentaine d'appels système avant même d'arriver dans `main` (chargement de la libc). C'est aussi pourquoi `file` décrit `bonjour` comme *statically linked* : il ne dépend d'aucune bibliothèque, et `ltrace` n'a rien à afficher.

## 1.4 Lire une entrée

[exemples/saluer.asm](../exemples/saluer.asm) lit un nom puis répond :

```nasm
section .data
    debut     db "Bonjour "
    l_debut   equ $ - debut

section .bss
    nom       resb 64           ; tampon de 64 octets non initialisé

section .text
    global _start
_start:
    ; n = read(0, nom, 64)
    mov eax, 0                  ; numéro de read
    mov edi, 0                  ; descripteur 0 : entrée standard
    lea rsi, [rel nom]          ; adresse du tampon
    mov edx, 64                 ; taille maximale
    syscall                     ; rax = nombre d'octets lus
    mov r12, rax                ; r12 n'est pas modifié par syscall

    ; write(1, debut, l_debut)
    mov eax, 1
    mov edi, 1                  ; descripteur 1 : sortie standard
    lea rsi, [rel debut]
    mov edx, l_debut
    syscall

    ; write(1, nom, n)
    mov eax, 1
    mov edi, 1
    lea rsi, [rel nom]
    mov rdx, r12
    syscall

    ; exit(0)
    mov eax, 60
    xor edi, edi
    syscall
```

```
$ echo Clemence | ./saluer
Bonjour Clemence
```

Trois remarques :

- le résultat de `read` (nombre d'octets lus, retour à la ligne compris) est mis de côté dans `r12`, car `rax` sera réutilisé pour le numéro de l'appel suivant ;
- `mov edi, 1` suffit pour mettre `rdi` à 1, puisque l'écriture dans un registre 32 bits efface la moitié haute (partie 2, section 2.6) ; c'est plus court à encoder, et c'est ce que fait gcc ;
- `read` ne termine pas la chaîne par un 0 : c'est pour cela que la longueur doit être conservée.

Dernière remarque sur l'adressage : `bonjour.asm` écrivait `mov rsi, message` (adresse absolue), alors que `saluer.asm` écrit `lea rsi, [rel nom]` (adresse relative à `rip`). Les deux donnent l'adresse d'une donnée. La forme `[rel ...]` est préférable car elle fonctionne quel que soit l'endroit où le programme est chargé en mémoire (code indépendant de la position, chapitre 6 de la partie 2) ; c'est aussi celle qu'exige la libc au chapitre 2.

## 1.5 Les arguments de la ligne de commande

Sans libc, personne ne prépare `argc` et `argv` dans `rdi` et `rsi`. Au début de `_start`, le noyau les a placés **sur la pile** :

| Adresse | Contenu |
|---|---|
| `[rsp]` | `argc` (8 octets) |
| `[rsp+8]` | `argv[0]` : adresse du nom du programme |
| `[rsp+16]` | `argv[1]` : adresse du premier argument |
| ... | ... |
| `[rsp+8*(argc+1)]` | 0, qui termine `argv` |
| au-delà | les variables d'environnement, puis les chaînes elles-mêmes |

Ainsi, `mov rsi, [rsp+16]` charge l'adresse de la chaîne `argv[1]`. Dans un programme gcc, c'est le code de démarrage de la libc qui lit cette pile et appelle `main(argc, argv, envp)`.

## 1.6 Le lien avec le reverse

**Reconnaître du code écrit à la main.** Un binaire sans `main`, sans libc, très petit, où les appels système sont faits directement et où les conventions sont parfois ignorées (registres utilisés librement, pas de cadre de pile) a probablement été écrit en assembleur. C'est fréquent dans les crackmes "old school" et les shellcodes.

**Les appels système directs échappent à `ltrace` et à `LD_PRELOAD`.** Au chapitre 5 de la partie 3, l'anti-debug par `ptrace` passait par la fonction de la libc, qu'il était possible de remplacer. [exemples/detecte.asm](../exemples/detecte.asm) fait l'appel système lui-même :

```nasm
_start:
    ; ptrace(PTRACE_TRACEME, 0, 0, 0)
    mov eax, 101                ; numéro de ptrace
    xor edi, edi                ; PTRACE_TRACEME = 0
    xor esi, esi
    xor edx, edx
    xor r10d, r10d              ; 4e argument : r10, et non rcx
    syscall
    test rax, rax
    js .detecte                 ; résultat négatif : erreur, déjà tracé
```

Ici, `ltrace` ne voit rien et une fausse fonction `ptrace` chargée par `LD_PRELOAD` n'est jamais appelée. En revanche, `strace` montre l'appel (et le déclenche, puisque `strace` utilise lui-même `ptrace`) :

```
$ strace ./detecte
ptrace(PTRACE_TRACEME)                  = -1 EPERM (Operation not permitted)
write(1, "Debogueur detecte\n", 18Debogueur detecte
```

Dans GDB, `catch syscall ptrace` arrête le programme **deux fois** : à l'entrée dans l'appel système, puis à sa sortie. Au second arrêt, `rax` contient le résultat, que l'on peut remplacer :

```
(gdb) catch syscall ptrace
(gdb) run
Catchpoint 1 (call to syscall ptrace), 0x0000000000401010 in _start ()
(gdb) continue
Catchpoint 1 (returned from syscall ptrace), 0x0000000000401010 in _start ()
(gdb) p $rax
$1 = -1
(gdb) set $rax = 0
(gdb) continue
Aucun debogueur
```

C'est la technique 2 de la section 5.2 de la partie 3, appliquée pas à pas. Dans Ghidra, un appel système apparaît sous la forme `syscall()` ; il faut alors retrouver le numéro dans `rax` (ou `eax`) juste avant, et le chercher dans la table.

## Exercices

Ces exercices demandent d'écrire ou de modifier de l'assembleur NASM, assemblé avec `nasm -f elf64 fichier.asm -o f.o && ld f.o -o programme`. Solutions à part, dans [solutions/partie3.md](../solutions/partie3.md#chapitre-1).

### Exercice 1.1 - code de retour 42 (N1)

- **Fichier à rendre :** une copie modifiée de `bonjour.asm`
- **À produire :** la vérification avec `echo $?`

Modifier le programme pour qu'il se termine avec le code de retour 42.

### Exercice 1.2 - deux sorties (N1)

- **Fichier à rendre :** `deux_sorties.asm`

Afficher deux lignes différentes, la première sur la sortie standard et la seconde sur la sortie d'erreur. Vérifier avec `./programme 2>/dev/null`, qui masque la sortie d'erreur.

### Exercice 1.3 - compter les appels système (N2)

- **Fichier à rendre :** un programme C contenant `puts("Bonjour")`
- **À produire :** le nombre d'appels système de chaque version (comparer `strace -c ./programme` et `strace -c ./bonjour`), et lequel correspond à l'affichage

### Exercice 1.4 - contourner l'anti-debug (N3)

- **À faire :** contourner l'anti-debug de `detecte` de deux autres façons, en s'aidant de `objdump -d -M intel detecte` pour les adresses

1. poser un point d'arrêt sur l'instruction qui suit le `syscall`, puis modifier `rax` ;
2. poser un point d'arrêt sur le `js`, puis faire sauter l'exécution à l'instruction suivante avec `set $pc = adresse` (`$pc` désigne `rip`), ce qui revient à ignorer le saut.

Les trois exercices suivants forment une mini-série piscine : ils construisent une petite bibliothèque d'affichage sans libc, réutilisée par la suite.

!!! tip "L'avertissement `.note.GNU-stack`"
    Depuis les versions récentes de `ld`, un objet NASM sans section de pile explicite déclenche un avertissement (`missing .note.GNU-stack section implies executable stack`). Il est sans danger ici, mais pour une sortie propre, ajouter en fin de fichier : `section .note.GNU-stack noalloc noexec nowrite progbits`.

### Exercice 1.5 - ma_putchar (niveau 1)

- **Fichier à rendre :** `ma_putchar.asm`
- **Contraintes :** pas de libc, pas de section `.data` ; pousser l'octet sur la pile pour que `rsi` pointe dessus ; un seul `write` par appel

Écrire une routine qui affiche l'octet présent dans `dil` sur la sortie standard. L'utiliser pour afficher `A` puis un retour à la ligne.

### Exercice 1.6 - ma_putstr (niveau 2)

- **Fichier à rendre :** `ma_putstr.asm`
- **Contraintes :** interdit d'écrire la longueur en dur (elle doit être calculée) ; un seul `write` par chaîne

Recevoir dans `rdi` l'adresse d'une chaîne terminée par `\0`, calculer sa longueur avec une boucle, puis l'afficher en un seul `write`. La tester sur deux chaînes différentes.

### Exercice 1.7 - mini-cat (niveau 3)

- **Fichier à rendre :** `moncat.asm`
- **Contraintes :** pas de libc ; un tampon en section `.bss` ; bien traiter le retour de `read` (`0` = EOF, négatif = erreur)

Lire l'entrée standard par blocs avec `read`, recopier chaque bloc sur la sortie standard avec `write`, et s'arrêter quand `read` renvoie 0. Tester avec `printf 'un\ndeux\n' | ./moncat` et avec `./moncat < fichier.txt`.

## Fil rouge partie 4 : réécrire le keygen du coffre v3

Au chapitre 4 de la partie 3, le coffre v3 a été vaincu par un keygen écrit en Python ([keygen_v3.py](../solutions/scripts/keygen_v3.py)). Le fil rouge de la partie 4 consiste à écrire ce même keygen **en assembleur**, en deux temps.

Étape 1 (ce chapitre) : écrire `fr15.asm`, sans libc, qui :

1. vérifie que `argc` vaut 2, et sinon affiche `usage : fr15 <nom>` sur la sortie d'erreur et quitte avec le code 1 ;
2. calcule la longueur de `argv[1]` avec une boucle (chapitre 4 de la partie 2) ;
3. affiche `argv[1]` suivi d'un retour à la ligne, puis quitte avec le code 0.

```
$ ./fr15 Clemence
Clemence
$ ./fr15
usage : fr15 <nom>
```

La boucle de longueur sera réutilisée au chapitre 2, où elle deviendra la boucle de hachage.

Solutions : [solutions/partie3.md](../solutions/partie3.md#chapitre-1)

## Suivi

- [ ] Partie 4, chapitre 1 : lecture
- [ ] Partie 4, chapitre 1 : exercices
- [ ] Partie 4, chapitre 1 : fil rouge
