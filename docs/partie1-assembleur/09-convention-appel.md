# La convention d'appel en profondeur

!!! info "Chapitre d'approfondissement"
    Ce chapitre vient après le challenge de synthèse. Il reprend la convention d'appel vue au chapitre 5 et la détaille entièrement. C'est de la matière avancée, précieuse pour le reverse sérieux : savoir exactement où passent les arguments permet de reconstruire la signature d'une fonction à partir de son seul code machine.

Au chapitre 5, on a vu l'essentiel : les premiers arguments arrivent dans `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`, le résultat repart dans `rax`, et la pile doit être alignée. Ce chapitre comble tous les trous de cette règle, qui porte un nom : la **System V AMD64 ABI**, la convention utilisée par Linux et macOS (Windows a la sienne, différente).

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
    Garder une valeur importante dans `rdi` « pendant » un `call` à `printf` : `printf` est libre d'écraser `rdi`. Il faut soit la mettre dans un registre *callee-saved* (après l'avoir sauvé), soit la ranger sur la pile. C'est exactement le piège de l'exercice 16.6 (partie 3).

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

## Exercices gradués

Tous se compilent en liant un objet NASM avec un programme C : `nasm -f elf64 ex.asm -o ex.o && gcc -no-pie test.c ex.o -o test`.

**Exercice 9.1 (niveau 1)** Écrire `long septieme(long a, ..., long g)` qui renvoie son septième argument. Vérifier depuis C que `septieme(1,2,3,4,5,6,777)` renvoie `777`.

!!! note "Contraintes"
    Le 7e argument n'est pas dans un registre : le lire sur la pile, au bon décalage par rapport à `rsp` à l'entrée de la fonction.

**Exercice 9.2 (niveau 2)** Écrire `long somme8(a, b, c, d, e, f, g, h)` qui additionne ses huit arguments (six en registres, deux sur la pile). Vérifier `somme8(1..8) == 36`.

**Exercice 9.3 (niveau 2)** Écrire `double moyenne(double a, double b)` qui renvoie `(a+b)/2`, en travaillant dans les registres `xmm`. Vérifier `moyenne(3.0, 8.0) == 5.50`.

!!! note "Contraintes"
    Arguments dans `xmm0` et `xmm1`, résultat dans `xmm0`. Utiliser `addsd` et `divsd`.

**Exercice 9.4 (niveau 3)** Écrire un `main` en assembleur qui affiche un entier et un flottant avec un seul `printf`. Faire fonctionner la version correcte (`al = 1`), puis mettre `al = 0` et constater que le flottant s'affiche faux. Expliquer pourquoi.

**Exercice 9.5 (niveau 2)** Écrire `struct P { long x, y; } paire(long a, long b)` qui renvoie `{a, 2*b}`, et vérifier depuis C que les deux champs sont corrects. Dans quels registres chaque champ revient-il ?

**Exercice 9.6 (niveau 3)** Écrire une fonction feuille `long rouge(long a, long b)` qui range ses deux arguments dans la red zone (`[rsp-8]`, `[rsp-16]`) **sans** modifier `rsp`, puis renvoie leur somme. Vérifier, et expliquer pourquoi cela resterait valide ici mais deviendrait faux si la fonction appelait une autre fonction.

Solutions : [solutions/partie1.md](../solutions/partie1.md#convention-appel)

## Suivi

- [ ] Chapitre 9 (convention d'appel) : lecture
- [ ] Chapitre 9 (convention d'appel) : exercices
