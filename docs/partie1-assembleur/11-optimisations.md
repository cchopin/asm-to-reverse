# Lire du code optimisé

!!! info "Chapitre d'approfondissement"
    Jusqu'ici, tous les désassemblages étaient produits avec `-O0` (aucune optimisation), volontairement proches du C. Mais en CTF et en analyse de malware, le code est presque toujours **optimisé** (`-O2`, voire `-O3`). Ce chapitre apprend à reconnaître ce que le compilateur fait alors, pour ne pas être désorienté par un code qui ne ressemble plus du tout au source.

Tous les listings de ce chapitre sont réels : ils ont été produits avec `gcc` puis `objdump -d -M intel`. On peut les reproduire.

## 11.1 Plus de cadre de pile, tout en registres

En `-O0`, chaque variable a un emplacement sur la pile, relu à chaque usage. En `-O2`, les variables vivent dans des registres et le cadre de pile disparaît souvent. Comparons `int fois18(int x) { return x * 18; }` :

```asm
; -O0
push rbp
mov  rbp, rsp
mov  [rbp-0x4], edi
mov  edx, [rbp-0x4]
mov  eax, edx
shl  eax, 0x3        ; x * 8
add  eax, edx        ; + x  -> x*9
add  eax, eax        ; *2   -> x*18
pop  rbp
ret
```

```asm
; -O2
lea  eax, [rdi+rdi*8] ; x*9 en une instruction
add  eax, eax         ; *2 -> x*18
ret
```

Le `-O2` n'a ni pile, ni va-et-vient mémoire : trois instructions au lieu de neuf, le résultat directement construit. C'est le premier choc en lisant du code optimisé : il faut suivre les valeurs dans les registres, sans le repère rassurant des `[rbp-...]`.

## 11.2 La réduction de force (strength reduction)

Les opérations lentes sont remplacées par des plus rapides. On vient de le voir pour la multiplication : `x * 18` devient `lea [rdi+rdi*8]` suivi d'un `add`. Plus généralement, `lea` et les décalages (`shl`) remplacent les multiplications par des constantes.

Le cas le plus déroutant est la **division par une constante**, remplacée par une multiplication. Soit `unsigned div3(unsigned x) { return x / 3; }` :

```asm
mov  eax, edi
mov  edx, 0xaaaaaaab   ; "nombre magique"
imul rax, rdx
shr  rax, 0x21         ; décalage de 33
ret
```

Il n'y a aucune division ! Le compilateur multiplie par un **nombre magique** (`0xAAAAAAAB`, proche de `2^33 / 3`) puis décale à droite. C'est mathématiquement équivalent à diviser par 3, mais bien plus rapide. Reconnaître ce motif (`imul` par une grosse constante apparemment arbitraire, suivi d'un `shr`) est essentiel : cela signifie « division par une constante », et le diviseur se retrouve en calculant `2^décalage / magique`.

## 11.3 Les conditionnelles sans saut

Un `if` simple ou un `? :` devient souvent une instruction conditionnelle, sans branchement, plus rapide pour le processeur. Soit `int maxi(int a, int b) { return a > b ? a : b; }` :

```asm
cmp    esi, edi
mov    eax, edi        ; résultat = a
cmovge eax, esi        ; si b >= a, résultat = b
ret
```

`cmovge` copie `esi` dans `eax` seulement si la condition est vraie : pas de saut, pas de `jmp`. De même, `setcc` pose un `0` ou un `1` dans un octet selon un drapeau. Quand on cherche un `if` et qu'on ne trouve aucun saut, c'est qu'il a été transformé en `cmov` ou `setcc`.

## 11.4 Reconnaître une expression recomposée

L'optimiseur combine plusieurs opérations en une. `int carre_plus(int x) { return x*x + 2*x + 1; }` donne :

```asm
mov  eax, edi
imul eax, edi            ; x*x
lea  eax, [rax+rdi*2+0x1] ; + 2*x + 1
ret
```

Le `lea` additionne `x*x`, `2*x` et `1` d'un seul coup. En lisant à l'envers, on reconstruit l'expression (ici `(x+1)²`). Les `lea` à trois termes sont presque toujours de l'arithmétique condensée, pas des accès mémoire.

## 11.5 La vectorisation

En `-O3`, une boucle qui traite un tableau peut être **vectorisée** : le processeur traite plusieurs éléments à la fois dans des registres `xmm` de 128 bits (SSE) ou `ymm` de 256 bits (AVX). La boucle `long somme(const int *t, long n)` compilée en `-O3` contient par exemple :

```asm
pxor   xmm0, xmm0        ; accumulateur mis à zéro
...
movdqu xmm1, [rax]       ; charge 4 entiers d'un coup
...
paddq  xmm0, xmm3        ; additionne en parallèle
```

Les instructions commençant par `p` (`paddq`, `pcmpgtd`, `punpckldq`...) et l'usage de `movdqu`/`xmm` signalent du code vectorisé. Il n'est pas nécessaire d'en décortiquer chaque instruction : il suffit de comprendre que la boucle additionne plusieurs éléments par tour, et qu'un petit bloc scalaire à la fin traite les éléments restants (le *reste* quand la taille n'est pas un multiple de la largeur vectorielle). Reconnaître « c'est une somme vectorisée » suffit la plupart du temps.

## 11.6 Inlining et disparition de fonctions

En optimisé, les petites fonctions sont souvent **inlinées** : leur corps est recopié chez l'appelant, et la fonction elle-même disparaît du binaire. On peut donc chercher en vain une fonction vue dans le source : son code est fondu dans celui qui l'appelait. À l'inverse, du code dupliqué à plusieurs endroits peut être le même petit utilitaire inliné partout.

## 11.7 Stratégie de lecture

Face à du code `-O2`/`-O3` :

1. Ne pas chercher le cadre de pile : suivre les registres.
2. Traduire mentalement les motifs : `lea`/`shl` = multiplication, `imul` par constante bizarre + `shr` = division, `cmov`/`setcc` = condition, `p...`/`xmm` = boucle vectorisée.
3. Reconstruire l'expression de haut niveau à partir du résultat, pas instruction par instruction.
4. Se souvenir que des fonctions ont pu être inlinées ou, au contraire, dupliquées.

C'est un entraînement : plus on lit de code optimisé, plus ces motifs deviennent immédiats.

## Exercices gradués

Compiler aux deux niveaux : `gcc -O0 -c f.c -o f0.o` et `gcc -O2 -c f.c -o f2.o`, puis `objdump -d -M intel`.

**Exercice 11.1 (niveau 1)** Compiler `int fois18(int x){ return x*18; }` en `-O0` et `-O2`. Expliquer, instruction par instruction, comment la version `-O2` calcule `x*18` avec un `lea` et un `add`.

**Exercice 11.2 (niveau 2)** Compiler `unsigned div3(unsigned x){ return x/3; }` en `-O2`. Il n'y a pas de division : identifier le nombre magique et le décalage, et vérifier que `magique ≈ 2^décalage / 3`.

**Exercice 11.3 (niveau 2)** Compiler `int maxi(int a,int b){ return a>b?a:b; }` en `-O2`. Repérer l'instruction conditionnelle utilisée et expliquer pourquoi il n'y a aucun saut.

**Exercice 11.4 (niveau 2)** Compiler `int carre_plus(int x){ return x*x+2*x+1; }` en `-O2`. Montrer comment les deux instructions produites recomposent l'expression, et retrouver sa forme factorisée.

**Exercice 11.5 (niveau 3)** Compiler `long somme(const int *t,long n){ long s=0; for(long i=0;i<n;i++) s+=t[i]; return s; }` en `-O3`. Identifier les instructions vectorielles (`xmm`, `movdqu`, `paddq`) et expliquer en une phrase ce que fait la boucle principale, puis à quoi sert le bloc scalaire final.

**Exercice 11.6 (niveau 3, lecture)** On lit dans un binaire : `imul rax, 0xCCCCCCCD` puis `shr rax, 0x23` (35). Quelle division par constante ce code réalise-t-il ? (Indice : `2^35 / 0xCCCCCCCD`.)

Solutions : [solutions/partie1.md](../solutions/partie1.md#optimisations)

## Suivi

- [ ] Chapitre 11 (code optimisé) : lecture
- [ ] Chapitre 11 (code optimisé) : exercices
