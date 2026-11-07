# Solutions de la partie 3

À ne consulter qu'après avoir cherché. Les scripts et fichiers sources cités sont dans le dossier `scripts` (liens au fil du texte) et dans [fil-rouge/partie3/chargeur.c](../fil-rouge/partie3/chargeur.c).

## Chapitre 15

**15.1** Remplacer `xor rdi, rdi` par `mov rdi, 42` (ou `mov edi, 42`) avant le `exit`. Après `./bonjour`, la commande `echo $?` affiche `42`. Le code de retour est le premier argument d'`exit`, donc `rdi`.

**15.2** Deux appels `write`, l'un avec `edi = 1`, l'autre avec `edi = 2` :

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

**15.3** `strace -c ./bonjour` compte 2 appels système (`write` et `exit`). `strace -c ./programme` en compte une trentaine : l'essentiel sert à charger la libc (`openat`, `mmap`, `mprotect`...) avant `main`. Dans les deux cas, un seul `write` correspond à l'affichage. C'est l'illustration du coût du démarrage de la libc, qu'un programme écrit à la main évite.

**15.4**
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

Le point délicat est la lecture de `argc` et `argv` directement sur la pile (section 15.5), puisque la libc n'est pas là pour les préparer.

## Chapitre 16

**16.1** La fonction, avec `cmovl` (déplacement conditionnel si inférieur, signé) :

```nasm
maximum:
    mov rax, rdi                ; résultat provisoire : a
    cmp rdi, rsi
    cmovl rax, rsi              ; si a < b, résultat = b
    ret
```

Appelée depuis C comme `somme3`. Avec des valeurs négatives, `cmovl` (signé) donne le bon résultat : `maximum(-5, -2)` renvoie `-2`. Un `cmovb` (non signé) se tromperait, car `-2` vaut `0xffff...fe`, considéré comme très grand en non signé. C'est le même piège que `jl` contre `jb` (chapitre 3).

**16.2** Le `main` reçoit `argc` dans `edi` :

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

**16.3** À l'entrée de `main`, `p/x $rsp` se termine par `8` : après le `call` de la libc, `rsp` vaut `16k + 8`. Appeler `printf` dans cet état plante dans un `movaps` de `printf`. En ajoutant `push rbx` au début (et `pop rbx` avant `ret`), `rsp` perd 8 octets et redevient multiple de 16 : `p/x $rsp` se termine alors par `0` juste avant le `call`, et le programme fonctionne.

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

## Chapitre 17

**17.1** Après `objcopy -O binary -j .text flag flag.bin`, la commande `python3 -c "print(open('flag.bin','rb').read().count(0))"` affiche `0`. En changeant le message (en respectant des blocs de 8 octets et en ajustant la taille passée à `write`), le lanceur affiche le nouveau texte, preuve que les octets sont autonomes.

**17.2** Shellcode minimal pour `exit(42)`, sans octet nul :

```nasm
_start:
    push 42
    pop rdi                     ; rdi = 42
    push 60
    pop rax                     ; rax = 60 (exit)
    syscall
```

Soit 8 octets, 0 nul. Les `push imm8 / pop` évitent les `mov` avec constante sur 4 octets, qui introduiraient des zéros. Tester avec le lanceur puis `echo $?` affiche `42`.

**17.3** `mov rax, 59` est encodé par NASM en `b8 3b 00 00 00` (`mov eax, 0x3b`) : la constante 59 tient sur 4 octets, dont **3 sont nuls**. `push 59 / pop rax` donne `6a 3b 58`, soit 3 octets sans aucun zéro. C'est pourquoi les shellcodes évitent systématiquement les `mov` avec petite constante.

**Fil rouge (analyse du loader)** Script de correction [scripts/decode_chargeur.py](scripts/decode_chargeur.py).

1. `strings chargeur | grep -i flag` ne renvoie rien : le flag est chiffré.
2. Dans `objdump -d -M intel chargeur`, on repère : `call mmap@plt`, puis une boucle contenant `movzx edx, BYTE PTR [rcx+rax*1]` / `xor edx, 0xffffffa7` / `mov BYTE PTR [rbx+rax*1], dl` bornée par `cmp rax, 0x41`, puis `call mprotect@plt`, et enfin `call rbx`. Le `lea rcx, [rip+0x...]` qui précède la boucle donne l'adresse du tableau (`0x2020`).
3. Méthode dynamique : `break *<adresse du call rbx>`, `run`, `x/20i $rbx`. Le shellcode déchiffré affiche `FLAG{chargeur_dechiffre}`.
4. Méthode statique : le script lit le fichier, extrait `0x41` octets à l'offset `0x2020`, applique `^ 0xA7` (l'octet bas de `0xffffffa7`), et désassemble avec pwntools. On y retrouve la construction de la chaîne sur la pile puis le `write`.
5. `dump binary memory` dans GDB et le fichier produit par le script sont identiques (`cmp` ne signale aucune différence) : l'analyse statique n'a rien manqué, et le malware n'a jamais eu besoin d'être réellement exécuté.
