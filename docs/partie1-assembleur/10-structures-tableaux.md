# Structures et tableaux en assembleur

!!! info "Chapitre d'approfondissement"
    Suite du chapitre 9. On a vu au chapitre 6 qu'une donnée en mémoire se lit par `[base + décalage]`. Ce chapitre généralise : comment une structure et un tableau de structures sont réellement disposés en mémoire, et comment reconnaître `p->champ` ou `tab[i].champ` dans un désassemblage. C'est un réflexe indispensable pour reconstruire les types d'un programme.

## 10.1 La disposition d'une structure

Une structure est une suite de champs placés en mémoire les uns après les autres, mais pas n'importe comment : chaque champ est **aligné** sur sa taille (un `int` de 4 octets à une adresse multiple de 4, un `long` de 8 octets à un multiple de 8). Le compilateur insère au besoin des octets de **remplissage** (*padding*), et arrondit la taille totale pour que les tableaux de la structure restent alignés.

```c
struct Enr { int id; long val; };
```

Ici `id` est à l'offset 0 (4 octets), puis 4 octets de padding, puis `val` à l'offset 8. Taille totale : 16 octets, et non 12. Le padding entre `id` et `val` existe pour que `val` tombe sur un multiple de 8.

| Champ | Offset | Taille |
|---|---|---|
| `id` | 0 | 4 |
| *(padding)* | 4 | 4 |
| `val` | 8 | 8 |

!!! tip "Vérifier soi-même"
    En C, `sizeof(struct Enr)` et `offsetof(struct Enr, val)` (via `<stddef.h>`) donnent la vérité. En reverse, ce sont les offsets lus dans le code qui révèlent la disposition, et donc les types.

## 10.2 Accéder à un champ

Accéder à `p->champ` revient toujours à `[base + offset_du_champ]`, où `base` est le pointeur sur la structure.

```nasm
; int lire_y(Point *p)   avec struct Point { int x, y; }  -> y a l'offset 4
lire_y:
    mov eax, [rdi+4]
    ret
```

Pour un champ plus petit que 8 octets, on complète comme toujours avec `movzx` (non signé) ou `movsx` / `movsxd` (signé), vus au chapitre 6.

## 10.3 Un tableau de structures

Un tableau range ses éléments bout à bout. L'élément `i` commence donc à `base + i * taille_élément`, et son champ est à `base + i * taille_élément + offset_du_champ`.

```nasm
; long somme_val(Enr *t, long n)   somme des t[i].val   (taille 16, val@8)
somme_val:
    xor eax, eax
    xor ecx, ecx
.b:
    cmp rcx, rsi
    jge .f
    mov rdx, rcx
    sal rdx, 4             ; i * 16  (décalage de 4 = multiplication par 16)
    add rax, [rdi+rdx+8]   ; t[i].val
    inc rcx
    jmp .b
.f:
    ret
```

Reconnaître ce motif est capital : un `[base + index*constante + petit_offset]` dans une boucle signale presque toujours un **tableau de structures**, et la constante donne la taille de l'élément, le petit offset le champ visé.

## 10.4 lea, le couteau suisse de l'adressage

`lea` (*load effective address*) calcule une adresse sans accéder à la mémoire. L'adressage x86-64 sait faire `base + index*échelle + déplacement`, où l'échelle vaut 1, 2, 4 ou 8. C'est exactement la forme des accès à des tableaux de `short`, `int`, `long` ou de pointeurs.

```nasm
lea rax, [rdi + rcx*4]     ; adresse de tab[rcx] pour un tableau d'int
```

Le compilateur utilise aussi `lea` pour de l'arithmétique pure (par exemple `lea rax, [rdi + rdi*2]` calcule `3*rdi` sans toucher à la mémoire). Voir un `lea` ne veut donc pas toujours dire « accès mémoire » : c'est souvent un simple calcul.

## 10.5 Les tableaux à deux dimensions

En C, un tableau `m[L][C]` est stocké **ligne par ligne** (ligne-majeur) : l'élément `m[i][j]` se trouve à l'indice linéaire `i * C + j`. L'adresse est donc `base + (i*ncol + j) * taille`.

```nasm
; int element(int *m, int ncol, int i, int j)
element:
    mov eax, edx          ; i
    imul eax, esi         ; i * ncol
    add eax, ecx          ; + j
    cdqe                  ; étendre l'index en 64 bits (signé)
    mov eax, [rdi+rax*4]  ; m[i*ncol+j]
    ret
```

Le `cdqe` (ou `movsxd`) qui étend un index 32 bits avant un accès mémoire est un détail fréquent du code gcc : l'index est calculé en `int` mais l'adressage se fait en 64 bits.

## 10.6 Lire une structure dans un désassemblage

C'est l'exercice inverse, et le plus utile en reverse. Soit la fonction :

```asm
mystere:
    movsxd rax, DWORD PTR [rdi]        ; champ à l'offset 0, int signé (4 o)
    add    rax, QWORD PTR [rdi+0x8]    ; champ à l'offset 8, long (8 o)
    movzx  edx, BYTE PTR [rdi+0x4]     ; champ à l'offset 4, octet non signé
    add    rax, rdx
    ret
```

Sans voir la source, on reconstruit la structure : un `int` à l'offset 0, un `char` (non signé, car `movzx`) à l'offset 4, un `long` à l'offset 8, le tout d'une taille de 16 octets. On en déduit `struct { int a; unsigned char b; long c; }`. Les tailles d'accès (`DWORD`, `BYTE`, `QWORD`) et les extensions (`movsxd` signé, `movzx` non signé) donnent les types exacts. C'est ainsi que Ghidra propose des types, et savoir le faire à la main permet de le corriger.

## Exercices

Ces exercices demandent d'écrire des fonctions en assembleur NASM. Compilation : `nasm -f elf64 ex.asm -o ex.o && gcc -no-pie test.c ex.o -o test`. Les structures sont définies côté C et passées par pointeur. Solutions à part, dans [solutions/partie1.md](../solutions/partie1.md#structures-tableaux).

### Exercice 10.1 - lire_y (niveau 1)

- **Fichier à rendre :** `lire_y.asm`
- **Prototype :** `int lire_y(Point *p);` avec `struct Point { int x, y; }`

Renvoyer `p->y`. Vérifier qu'il lit bien l'offset 4.

### Exercice 10.2 - double_x (niveau 2)

- **Fichier à rendre :** `double_x.asm`
- **Prototype :** `void double_x(Point *p);`

Doubler `p->x` **en place** (modification via le pointeur). Vérifier depuis C que la structure est modifiée.

### Exercice 10.3 - somme_val (niveau 2)

- **Fichier à rendre :** `somme_val.asm`
- **Prototype :** `long somme_val(Enr *t, long n);` avec `struct Enr { int id; long val; }`
- **Contraintes :** l'adresse de `t[i].val` se calcule comme `base + i*taille + offset`. Ne pas supposer que la taille vaut 12 : vérifier avec `sizeof`.

Additionner les `val` des `n` éléments. Déterminer d'abord la taille d'un `Enr` et l'offset de `val`.

### Exercice 10.4 - element 2D (niveau 3)

- **Fichier à rendre :** `element.asm`
- **Prototype :** `int element(int *m, int ncol, int i, int j);`
- **Contraintes :** étendre l'index en 64 bits (`cdqe` ou `movsxd`) avant l'accès mémoire. Utiliser l'échelle `*4` de l'adressage.

Renvoyer `m[i][j]` d'un tableau 2D ligne-majeur. Tester avec un `int m[2][3]`.

### Exercice 10.5 - lire_flags (niveau 2)

- **Fichier à rendre :** `lire_flags.asm`
- **Prototype :** `int lire_flags(Drapeau *p);` avec `struct Drapeau { unsigned char flags; int n; }`

Renvoyer `flags` étendu à un `int`. Pourquoi `movzx` et non `movsx` ?

### Exercice 10.6 - reconstruire une structure (niveau 3, lecture)

- **À produire :** l'offset, la taille et le caractère signé de chaque champ, puis une déclaration C compatible

Reconstruire la structure manipulée par la fonction `mystere` de la section 10.6.

## Suivi

- [ ] Partie 2, chapitre 10 (structures et tableaux) : lecture
- [ ] Partie 2, chapitre 10 (structures et tableaux) : exercices
