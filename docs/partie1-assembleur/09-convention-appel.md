# La convention d'appel en profondeur

!!! info "Chapitre d'approfondissement"
    Ce chapitre vient après le challenge de synthèse. Il reprend la convention d'appel vue au chapitre 5 et la détaille entièrement. C'est de la matière avancée, précieuse pour le reverse sérieux : savoir exactement où passent les arguments permet de reconstruire la signature d'une fonction à partir de son seul code machine.

Au chapitre 5, on a vu l'essentiel : les premiers arguments arrivent dans `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`, le résultat repart dans `rax`, et la pile doit être alignée. Ce chapitre comble tous les trous de cette règle, qui porte un nom : la **System V AMD64 ABI**, la convention utilisée par Linux et macOS (Windows a la sienne, différente).

## Avant de commencer : écrire une petite fonction en NASM

Les exercices des chapitres 9 à 13 demandent d'**écrire** de courtes fonctions en assembleur et de les appeler depuis un programme C, pour vérifier une règle en machine. L'écriture d'assembleur est l'objet de la partie 4, mais ces exercices n'en demandent qu'une toute petite partie, résumée ici. Comme à la section 2.6, on utilise l'assembleur NASM (syntaxe Intel), installé dans le conteneur.

Une fonction NASM appelable depuis C tient en quelques lignes :

```nasm
; double.asm : long double_de(long x), appelable depuis C
section .text
    global double_de        ; rend le nom visible pour l'édition de liens
double_de:
    lea rax, [rdi+rdi]      ; argument 1 dans rdi, résultat dans rax
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
```

- `section .text` ouvre la zone du code ; `section .data` ou `.rodata` servent aux données, si besoin.
- `global double_de` exporte le nom : sans cette ligne, le C ne trouve pas la fonction.
- l'étiquette `double_de:` marque le début de la fonction, `ret` sa fin. On applique la convention d'appel du chapitre 5 : arguments dans `rdi`, `rsi`..., résultat dans `rax`.
- la dernière ligne évite un avertissement de l'éditeur de liens (elle déclare que la pile n'a pas besoin d'être exécutable). On la recopie telle quelle à la fin de chaque fichier.

Le programme C de test déclare le prototype, puis appelle la fonction :

```c
#include <stdio.h>

long double_de(long x);     /* définie dans double.asm */

int main(void)
{
    printf("%ld\n", double_de(21));
    return 0;
}
```

On assemble le `.asm` en fichier objet, puis `gcc` compile le C et lie les deux :

```bash
nasm -f elf64 double.asm -o double.o
gcc -no-pie test.c double.o -o test
./test                      # affiche 42
```

En cas d'erreur, GDB (section 2.6) permet de suivre la fonction instruction par instruction : `break double_de`, `run`, puis `si` et `p $rax`.

Un seul exercice (9.4) écrit un `main` complet en assembleur qui appelle `printf`. Trois lignes de plus suffisent : `extern printf` déclare la fonction de la libc, `call printf wrt ..plt` l'appelle en passant par la PLT (chapitre 5), et un `push rbp` en début de `main` (avec le `pop rbp` correspondant avant `ret`) remet la pile alignée sur 16 octets, comme l'exige la convention avant tout `call`. La raison de cet alignement est détaillée au chapitre 2 de la partie 4 ; ici, il suffit de suivre le modèle :

```nasm
; aff.asm : un main en assembleur qui appelle printf
default rel                 ; les accès aux données sont relatifs à rip (chapitre 6)
extern printf
section .rodata
    fmt db "valeur : %d", 10, 0
section .text
    global main
main:
    push rbp                ; réaligne rsp sur 16 avant le call
    lea rdi, [fmt]
    mov esi, 42
    xor eax, eax            ; aucun registre xmm utilisé (voir 9.5)
    call printf wrt ..plt
    xor eax, eax            ; return 0
    pop rbp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
```

Assemblage et édition de liens : `nasm -f elf64 aff.asm -o aff.o && gcc -no-pie aff.o -o aff`. Le programme affiche `valeur : 42`.

## 9.1 Les arguments entiers et pointeurs

Les six premiers arguments entiers ou pointeurs passent, dans l'ordre, par :

| Argument | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|
| Registre | `rdi` | `rsi` | `rdx` | `rcx` | `r8` | `r9` |

Au-delà de six, les arguments suivants sont **poussés sur la pile**, de droite à gauche, par l'appelant. Au moment où la fonction appelée démarre, la pile contient, de bas en haut : l'adresse de retour (poussée par `call`), puis le 7e argument, puis le 8e, etc.

```nasm
; long f(a, b, c, d, e, f, g)   -> g est le 7e argument
f:
    mov rax, [rsp+8]   ; g : juste au-dessus de l'adresse de retour
    ret
```

C'est la clé de lecture : dès qu'on voit une fonction lire `[rsp+8]`, `[rsp+16]`... sans les avoir écrits, c'est qu'elle a plus de six arguments.

## 9.2 La valeur de retour

Un entier ou un pointeur revient dans `rax`. Pour une valeur de 128 bits (par exemple un `__int128`, ou une petite structure, voir 9.6), le couple `rax:rdx` est utilisé : `rax` contient les 64 bits de poids faible, `rdx` les 64 bits de poids fort. Les valeurs à virgule flottante reviennent dans `xmm0`.

## 9.3 Registres préservés ou non

C'est le point le plus souvent négligé, et une source classique de bugs. Les registres se répartissent en deux familles :

| Famille | Registres | Qui en est responsable |
|---|---|---|
| *Caller-saved* (volatils) | `rax`, `rcx`, `rdx`, `rsi`, `rdi`, `r8`-`r11` | L'**appelant** les sauve s'il en a besoin après un `call` |
| *Callee-saved* (préservés) | `rbx`, `rbp`, `r12`-`r15`, `rsp` | La fonction **appelée** doit les rendre intacts |

Autrement dit : si une fonction veut utiliser `rbx` ou `r12`, elle doit les sauver (`push`) en entrée et les restaurer (`pop`) en sortie, car son appelant compte les retrouver inchangés. À l'inverse, après un `call`, on ne peut rien supposer sur `rax`, `rcx`, `rdi`... : ils ont pu être écrasés.

!!! warning "Le bug typique"
    Garder une valeur importante dans `rdi` « pendant » un `call` à `printf` : `printf` est libre d'écraser `rdi`. Il faut soit la mettre dans un registre *callee-saved* (après l'avoir sauvé), soit la ranger sur la pile. C'est exactement le piège de l'exercice 16.6 (partie 4).

## 9.4 La red zone

Juste en dessous de `rsp`, la convention réserve une zone de **128 octets** dans laquelle une fonction peut écrire librement sans décrémenter `rsp` : c'est la *red zone*. Le noyau et les gestionnaires de signaux garantissent de ne pas l'écraser.

Elle permet aux **fonctions feuilles** (qui n'appellent personne) d'utiliser un peu de mémoire temporaire sans le moindre `sub rsp` / `add rsp`. En reverse, une fonction qui accède à `[rsp-8]`, `[rsp-16]` sans avoir réservé d'espace exploite la red zone : c'est un indice qu'il s'agit d'une feuille optimisée.

```nasm
rouge:
    mov [rsp-8], rdi    ; stockage temporaire dans la red zone
    mov [rsp-16], rsi   ; aucun "sub rsp" nécessaire
    ...
```

Attention : dès qu'une fonction fait un `call`, elle ne peut plus compter sur la red zone au-delà de ce `call` (l'appel écrit à `[rsp-8]` l'adresse de retour).

## 9.5 Les flottants et les fonctions variadiques

Les huit premiers arguments à virgule flottante passent par `xmm0` à `xmm7`, indépendamment des registres entiers. Une fonction `void f(int n, double x)` reçoit donc `n` dans `edi` et `x` dans `xmm0`.

Les fonctions **variadiques** (comme `printf`) ont une règle supplémentaire : avant le `call`, le registre `al` doit contenir **le nombre de registres vectoriels (`xmm`) utilisés** pour les arguments. Cela permet à la fonction de savoir combien de `xmm` sauvegarder.

```nasm
    lea rdi, [rel fmt]     ; "..%d..%f.."
    mov esi, 42            ; %d
    movsd xmm0, [rel pi]   ; %f
    mov eax, 1             ; AL = 1 : un seul xmm utilisé
    call printf wrt ..plt
```

Oublier `mov eax, ...` (ou mettre `al = 0` alors qu'on passe un flottant) fait afficher n'importe quoi à la place du nombre : c'est un symptôme très reconnaissable. En reverse, un `mov eax, 0` (ou `xor eax, eax`) juste avant un `call` vers une fonction variadique signale qu'aucun flottant n'est passé.

## 9.6 Le retour de petites structures

Une petite structure (jusqu'à 16 octets, composée de champs entiers) n'est pas retournée par un pointeur caché : elle revient directement dans `rax:rdx`. Pour `struct { long x, y; }`, `x` est dans `rax` et `y` dans `rdx`.

```nasm
; struct P { long x, y; } paire(long a, long b)
paire:
    mov rax, rdi       ; x
    lea rdx, [rsi+rsi] ; y = 2*b
    ret
```

Au-delà de 16 octets, le mécanisme change : l'appelant réserve la place et passe un pointeur caché comme « argument 0 » dans `rdi` (les vrais arguments décalent alors d'un cran). Reconnaître ce pointeur caché explique bien des désassemblages déroutants.

## 9.7 Pourquoi c'est capital en reverse

Toute cette mécanique sert un objectif unique : **reconstruire la signature d'une fonction à partir de son code**. En lisant quels registres une fonction consomme avant tout calcul (`rdi`, puis `rsi`, puis `xmm0`...), combien d'arguments pile elle touche, ce qu'elle laisse dans `rax`, on retrouve son prototype C sans jamais voir la source. C'est ce que fait Ghidra automatiquement, et savoir le faire à la main permet de corriger ses erreurs.

## Exercices

Ces exercices demandent d'écrire des fonctions en assembleur NASM. Chacune se compile en liant l'objet NASM avec un programme C de test : `nasm -f elf64 ex.asm -o ex.o && gcc -no-pie test.c ex.o -o test`. Solutions à part, dans [solutions/partie1.md](../solutions/partie1.md#convention-appel).

### Exercice 9.1 - septieme (niveau 1)

- **Fichier à rendre :** `septieme.asm` (plus un `test.c`)
- **Prototype :** `long septieme(long a, long b, long c, long d, long e, long f, long g);`
- **Contraintes :** le 7e argument n'est pas dans un registre ; le lire sur la pile, au bon décalage par rapport à `rsp` à l'entrée de la fonction.

Renvoyer le septième argument. Vérifier depuis C que `septieme(1,2,3,4,5,6,777)` renvoie `777`.

### Exercice 9.2 - somme8 (niveau 2)

- **Fichier à rendre :** `somme8.asm`
- **Prototype :** `long somme8(long a, long b, long c, long d, long e, long f, long g, long h);`

Additionner les huit arguments (six en registres, deux sur la pile). Vérifier `somme8(1..8) == 36`.

### Exercice 9.3 - moyenne (niveau 2)

- **Fichier à rendre :** `moyenne.asm`
- **Prototype :** `double moyenne(double a, double b);`
- **Contraintes :** arguments dans `xmm0` et `xmm1`, résultat dans `xmm0`. Utiliser `addsd` et `divsd`.

Renvoyer `(a+b)/2`. Vérifier `moyenne(3.0, 8.0) == 5.50`.

### Exercice 9.4 - printf mixte (niveau 3)

- **Fichier à rendre :** `mixte.asm`

Écrire un `main` en assembleur qui affiche un entier et un flottant avec un seul `printf`. Faire fonctionner la version correcte (`al = 1`), puis mettre `al = 0` et constater que le flottant s'affiche faux. Expliquer pourquoi.

### Exercice 9.5 - paire (niveau 2)

- **Fichier à rendre :** `paire.asm`
- **Prototype :** `struct P { long x, y; } paire(long a, long b);`

Renvoyer `{a, 2*b}`, et vérifier depuis C que les deux champs sont corrects. Dans quels registres chaque champ revient-il ?

### Exercice 9.6 - red zone (niveau 3)

- **Fichier à rendre :** `rouge.asm`
- **Prototype :** `long rouge(long a, long b);`

Écrire une fonction feuille qui range ses deux arguments dans la red zone (`[rsp-8]`, `[rsp-16]`) **sans** modifier `rsp`, puis renvoie leur somme. Vérifier, et expliquer pourquoi cela resterait valide ici mais deviendrait faux si la fonction appelait une autre fonction.

## Suivi

- [ ] Partie 2, chapitre 9 (convention d'appel) : lecture
- [ ] Partie 2, chapitre 9 (convention d'appel) : exercices
