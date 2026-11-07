# Solutions de la partie 4

À ne consulter qu'après avoir cherché. Les scripts et fichiers sources cités sont dans le dossier `scripts` (liens au fil du texte) et dans [fil-rouge/partie3/chargeur.c](../fil-rouge/partie3/chargeur.c).

## Chapitre 1

**1.1** Remplacer `xor rdi, rdi` par `mov rdi, 42` (ou `mov edi, 42`) avant le `exit`. Après `./bonjour`, la commande `echo $?` affiche `42`. Le code de retour est le premier argument d'`exit`, donc `rdi`.

**1.2** Deux appels `write`, l'un avec `edi = 1`, l'autre avec `edi = 2` :

```nasm
section .data
    m db "sortie standard", 10
    e db "sortie d'erreur", 10
section .text
    global _start
_start:
    mov eax, 1
    mov edi, 1
    lea rsi, [rel m]
    mov edx, 16
    syscall
    mov eax, 1
    mov edi, 2
    lea rsi, [rel e]
    mov edx, 16
    syscall
    mov eax, 60
    xor edi, edi
    syscall
```

Avec `./programme 2>/dev/null`, seule la première ligne s'affiche : la seconde est partie sur le descripteur 2, qui a été redirigé vers le vide.

**1.3** `strace -c ./bonjour` compte 2 appels système (`write` et `exit`). `strace -c ./programme` en compte une trentaine : l'essentiel sert à charger la libc (`openat`, `mmap`, `mprotect`...) avant `main`. Dans les deux cas, un seul `write` correspond à l'affichage. C'est l'illustration du coût du démarrage de la libc, qu'un programme écrit à la main évite.

**1.4**
1. `objdump -d -M intel detecte` donne l'adresse de l'instruction qui suit le `syscall` (`test rax, rax`, ici `0x401010`). Dans GDB : `break *0x401010`, `run`, `set $rax = 0`, `continue`. Le programme affiche "Aucun debogueur".
2. L'adresse du `js` est `0x401013` et celle de l'instruction suivante `0x401015`. Dans GDB : `break *0x401013`, `run`, `set $pc = 0x401015`, `continue`. Le saut vers le bloc "détecté" est ainsi contourné.

**Fil rouge (étape 1)** Fichier [scripts/fil_rouge_15.asm](scripts/fil_rouge_15.asm). La structure attendue :

```nasm
_start:
    cmp qword [rsp], 2          ; argc
    jne .usage
    mov rsi, [rsp+16]           ; argv[1]
    xor edx, edx                ; longueur
.longueur:
    cmp byte [rsi+rdx], 0
    je .afficher
    inc rdx
    jmp .longueur
.afficher:
    mov eax, 1                  ; write(1, argv[1], longueur)
    mov edi, 1
    syscall
    ; ... write du retour à la ligne, puis exit(0)
```

Le point délicat est la lecture de `argc` et `argv` directement sur la pile (section 1.5), puisque la libc n'est pas là pour les préparer.

## Chapitre 2

**2.1** La fonction, avec `cmovl` (déplacement conditionnel si inférieur, signé) :

```nasm
maximum:
    mov rax, rdi                ; résultat provisoire : a
    cmp rdi, rsi
    cmovl rax, rsi              ; si a < b, résultat = b
    ret
```

Appelée depuis C comme `somme3`. Avec des valeurs négatives, `cmovl` (signé) donne le bon résultat : `maximum(-5, -2)` renvoie `-2`. Un `cmovb` (non signé) se tromperait, car `-2` vaut `0xffff...fe`, considéré comme très grand en non signé. C'est le même piège que `jl` contre `jb` (chapitre 3).

**2.2** Le `main` reçoit `argc` dans `edi` :

```nasm
main:
    push rbx                    ; alignement de la pile
    mov esi, edi                ; 2e argument de printf : argc
    lea rdi, [rel format]       ; "argc vaut %d\n"
    xor eax, eax                ; aucun registre vectoriel
    call printf wrt ..plt
    xor eax, eax
    pop rbx
    ret
```

`./programme a b c` affiche `argc vaut 4` (le nom du programme compte pour 1).

**2.3** À l'entrée de `main`, `p/x $rsp` se termine par `8` : après le `call` de la libc, `rsp` vaut `16k + 8`. Appeler `printf` dans cet état plante dans un `movaps` de `printf`. En ajoutant `push rbx` au début (et `pop rbx` avant `ret`), `rsp` perd 8 octets et redevient multiple de 16 : `p/x $rsp` se termine alors par `0` juste avant le `call`, et le programme fonctionne.

**Fil rouge (étape 2)** Fichier [scripts/keygen_v3.asm](scripts/keygen_v3.asm). La fonction de hachage reprend la boucle de l'étape 1, en remplaçant le comptage par le calcul :

```nasm
; unsigned int hacher(const char *nom)   nom dans rdi, résultat dans eax
hacher:
    mov eax, 0x1337             ; h = 0x1337
.boucle:
    movzx edx, byte [rdi]       ; c = *nom
    test dl, dl
    jz .fin
    imul eax, eax, 31           ; h = h * 31
    add eax, edx                ; h = h + c
    rol eax, 3                  ; rotation à gauche de 3 bits
    inc rdi
    jmp .boucle
.fin:
    xor eax, 0xC0FFEE
    ret
```

```
$ ./keygen Clemence
F6E88EDC
```

Résultat identique au keygen Python. Différences de style qui trahissent le code écrit à la main : `rol` (gcc préfère souvent `shl` + `shr` + `or`), l'absence de cadre de pile complet, et des registres choisis librement plutôt que selon les habitudes du compilateur. Un binaire gcc contiendrait aussi du code de démarrage et des protections (canari) absents ici.

## Chapitre 3

**3.1** Après `objcopy -O binary -j .text flag flag.bin`, la commande `python3 -c "print(open('flag.bin','rb').read().count(0))"` affiche `0`. En changeant le message (en respectant des blocs de 8 octets et en ajustant la taille passée à `write`), le lanceur affiche le nouveau texte, preuve que les octets sont autonomes.

**3.2** Shellcode minimal pour `exit(42)`, sans octet nul :

```nasm
_start:
    push 42
    pop rdi                     ; rdi = 42
    push 60
    pop rax                     ; rax = 60 (exit)
    syscall
```

Soit 8 octets, 0 nul. Les `push imm8 / pop` évitent les `mov` avec constante sur 4 octets, qui introduiraient des zéros. Tester avec le lanceur puis `echo $?` affiche `42`.

**3.3** `mov rax, 59` est encodé par NASM en `b8 3b 00 00 00` (`mov eax, 0x3b`) : la constante 59 tient sur 4 octets, dont **3 sont nuls**. `push 59 / pop rax` donne `6a 3b 58`, soit 3 octets sans aucun zéro. C'est pourquoi les shellcodes évitent systématiquement les `mov` avec petite constante.

**Fil rouge (analyse du loader)** Script de correction [scripts/decode_chargeur.py](scripts/decode_chargeur.py).

1. `strings chargeur | grep -i flag` ne renvoie rien : le flag est chiffré.
2. Dans `objdump -d -M intel chargeur`, on repère : `call mmap@plt`, puis une boucle contenant `movzx edx, BYTE PTR [rcx+rax*1]` / `xor edx, 0xffffffa7` / `mov BYTE PTR [rbx+rax*1], dl` bornée par `cmp rax, 0x41`, puis `call mprotect@plt`, et enfin `call rbx`. Le `lea rcx, [rip+0x...]` qui précède la boucle donne l'adresse du tableau (`0x2020`).
3. Méthode dynamique : `break *<adresse du call rbx>`, `run`, `x/20i $rbx`. Le shellcode déchiffré affiche `FLAG{chargeur_dechiffre}`.
4. Méthode statique : le script lit le fichier, extrait `0x41` octets à l'offset `0x2020`, applique `^ 0xA7` (l'octet bas de `0xffffffa7`), et désassemble avec pwntools. On y retrouve la construction de la chaîne sur la pile puis le `write`.
5. `dump binary memory` dans GDB et le fichier produit par le script sont identiques (`cmp` ne signale aucune différence) : l'analyse statique n'a rien manqué, et le malware n'a jamais eu besoin d'être réellement exécuté.

## Exercices gradués de la partie 4

### Chapitre 1

**1.5** Routine `ma_putchar` (l'octet est poussé sur la pile pour avoir une adresse) :

```nasm
ma_putchar:                 ; octet dans dil
    push rdi
    mov eax, 1              ; write
    mov edi, 1
    mov rsi, rsp           ; adresse de l'octet poussé
    mov edx, 1
    syscall
    pop rdi
    ret
```

Appelée avec `mov dil, 'A' / call ma_putchar`, puis `mov dil, 10 / call ma_putchar`, elle affiche `A` et un retour à la ligne.

**1.6** `ma_putstr` calcule la longueur puis écrit tout d'un coup :

```nasm
ma_putstr:                  ; rdi = adresse de la chaîne
    xor edx, edx
.len:
    cmp byte [rdi+rdx], 0
    je .write
    inc rdx
    jmp .len
.write:
    mov rsi, rdi
    mov eax, 1
    mov edi, 1
    syscall
    ret
```

**1.7** Mini-`cat` : boucle `read` / `write` jusqu'à EOF.

```nasm
section .bss
    buf resb 4096
section .text
global _start
_start:
.loop:
    xor eax, eax                ; read(0, buf, 4096)
    xor edi, edi
    lea rsi, [rel buf]
    mov edx, 4096
    syscall
    test rax, rax
    jle .end                    ; 0 = EOF, <0 = erreur
    mov rdx, rax
    mov eax, 1                  ; write(1, buf, lus)
    mov edi, 1
    lea rsi, [rel buf]
    syscall
    jmp .loop
.end:
    mov eax, 60
    xor edi, edi
    syscall
```

### Chapitre 2

**2.4** Fonction feuille :

```nasm
global cube
cube:                       ; long cube(long n)
    mov rax, rdi
    imul rax, rdi           ; n*n
    imul rax, rdi           ; n*n*n
    ret
section .note.GNU-stack noalloc noexec nowrite progbits
```

`cube(2)=8`, `cube(5)=125`, `cube(-3)=-27`.

**2.5** Somme d'un tableau de `long` :

```nasm
global somme_tableau
somme_tableau:              ; rdi = tableau, rsi = n
    xor eax, eax
    xor ecx, ecx
.boucle:
    cmp rcx, rsi
    jge .fin
    add rax, [rdi + rcx*8]
    inc rcx
    jmp .boucle
.fin:
    ret
```

Sur `{3, 7, -2, 10, 42}` avec `n = 5`, le résultat est `60`.

**2.6** `main` qui imprime chaque argument. Les trois `push` réalignent la pile (entrée à `16k+8`, trois `push` ramènent à `16k`) et sauvegardent les registres *callee-saved* :

```nasm
extern printf
section .data
    fmt db "%s", 10, 0
section .text
global main
main:                       ; edi = argc, rsi = argv
    push rbx
    push r12
    push r13
    mov r12d, edi           ; argc
    mov r13, rsi            ; argv
    xor ebx, ebx            ; i
.boucle:
    cmp ebx, r12d
    jge .fin
    mov rsi, [r13 + rbx*8]
    lea rdi, [rel fmt]
    xor eax, eax
    call printf wrt ..plt
    inc ebx
    jmp .boucle
.fin:
    pop r13
    pop r12
    pop rbx
    xor eax, eax
    ret
```

`./programme alpha beta gamma` affiche le nom du programme puis `alpha`, `beta`, `gamma`, chacun sur sa ligne.

### Chapitre 3

**3.5** `hey\n` tient sur 4 octets, poussés en une fois ; les petites constantes passent par `push imm8 / pop` :

```nasm
_start:
    push 1
    pop rax                 ; write
    push 1
    pop rdi                 ; fd = 1
    push 0x0a796568         ; "hey\n" (68 65 79 0a)
    mov rsi, rsp
    push 4
    pop rdx
    syscall
    push 60
    pop rax
    xor edi, edi
    syscall
```

`objcopy -O binary -j .text` puis `count(0)` donne `0` octet nul. Le lanceur affiche `hey`.

**3.6** `reverse\n` fait 8 octets, tous non nuls, donc `mov r64, imm64` ne crée aucun zéro :

```nasm
_start:
    push 1
    pop rax
    push 1
    pop rdi
    mov rbx, 0x0a65737265766572   ; "reverse\n"
    push rbx
    mov rsi, rsp
    push 8
    pop rdx
    syscall
    push 60
    pop rax
    xor edi, edi
    syscall
```

Une chaîne plus courte laisserait des octets hauts nuls dans la constante `imm64` : c'est pourquoi on complète souvent à 8 octets utiles, ou on construit la chaîne autrement.

**3.7** XOR roulant `cle_i = 0x5A ^ i`. On déchiffre à froid, puis on désassemble sans exécuter :

```python
enc = bytes.fromhex(
    "305a00335f0014e61c600f1e1d5d5455"
    "1903c1af2449164247297c1977b84b40")
dec = bytes(b ^ (0x5A ^ (i & 0xff)) for i, b in enumerate(enc))
open("octets.bin", "wb").write(dec)
```

Puis `objdump -D -b binary -m i386:x86-64 -M intel octets.bin`. On lit un `write(1, "N3_OK\n", 6)` (chaîne poussée via `mov rbx, 0x000a4b4f5f334e`) suivi d'un `exit(0)` : un shellcode inoffensif qui affiche simplement `N3_OK`. L'important est de l'avoir établi **sans jamais le lancer**.

## Challenge de synthèse {#challenge}

Fichier complet : [scripts/filtre_majuscules.asm](scripts/filtre_majuscules.asm). Le cœur est la boucle de transformation entre la lecture et l'écriture :

```nasm
    mov rcx, rax                ; octets lus
    xor rbx, rbx
.boucle:
    mov dl, [buf + rbx]
    cmp dl, 'a'
    jb .suivant
    cmp dl, 'z'
    ja .suivant
    sub dl, 0x20                ; minuscule -> majuscule
    mov [buf + rbx], dl
.suivant:
    inc rbx
    cmp rbx, rcx
    jl .boucle
```

Les deux `cmp` encadrent la plage `a`-`z` ; un octet hors plage saute directement à `.suivant`, inchangé. Le `sub dl, 0x20` est la conversion de casse (l'écart constant entre minuscules et majuscules en ASCII).

Tests :

```
$ printf 'Bonjour Clemence, reverse 2026!\n' | ./filtre_majuscules
BONJOUR CLEMENCE, REVERSE 2026!
$ printf 'abc\nxyz 123 fin\n' | ./filtre_majuscules
ABC
XYZ 123 FIN
```

Au désassemblage (`objdump -d -M intel`), on retrouve les trois phases attendues : `syscall` de lecture après `xor eax,eax / xor edi,edi`, la boucle avec `cmp` sur `0x61`/`0x7a` et `sub` de `0x20`, puis le `syscall` d'écriture avec `eax = 1`. Cette signature (deux bornes de 26 valeurs + `sub 0x20`) est reconnaissable instantanément comme une conversion de casse, même sans le code source.

**Variante rot13** : remplacer le `sub dl, 0x20` par un décalage de 13 avec repli dans l'alphabet (ramener d'abord la lettre à `0`-`25`, ajouter `13`, prendre le modulo `26`, puis rétablir la base `'a'` ou `'A'`). C'est le chiffrement symétrique le plus simple, et l'appliquer deux fois redonne le texte d'origine.
