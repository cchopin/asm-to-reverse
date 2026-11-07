# Solutions de la partie 1

Une solution ne se consulte qu'après une recherche sérieuse. Une réponse différente n'est pas forcément fausse : la vérifier en compilant ou dans GDB.

## Chapitre 1

**1.1** 200 = `0xC8`. `0x7F` = 127. `0b10110011` = `1011 0011` = `0xB3`.

**1.2** `0x80` signé sur 8 bits = -128 (le plus petit). `0xFE` = -2 (car `0xFF` = -1).

**1.3** `(0x2165737265766572).to_bytes(8, 'little')` donne `b'reverse!'`. Les octets en mémoire sont `72 65 76 65 72 73 65 21`.

**1.4** Si `0x55` est le chiffré de `'F'` (`0x46`), la clé est `0x55 ^ 0x46 = 0x13`. Déchiffré : `bytes(b ^ 0x13 for b in [0x55, 0x7f, 0x72, 0x74])` = `b'Flag'`. Astuce à retenir : avec un XOR, **connaître un octet en clair donne la clé** (attaque à clair connu). En CTF, on connaît souvent le début du flag (`FLAG{`, `HTB{`, `picoCTF{`) : c'est une arme redoutable.

**1.5**

```python
def xor(data: bytes, cle: int) -> bytes:
    return bytes(b ^ cle for b in data)

def xor_multi(data: bytes, cle: bytes) -> bytes:   # bonus : clé de plusieurs octets
    return bytes(b ^ cle[i % len(cle)] for i, b in enumerate(data))
```

**Fil rouge** La fonction `verifier` contient 64 instructions, 6 `cmp` et 6 `jmp`. Les constantes intéressantes :

| Constante | Décimal | ASCII |
|---|---|---|
| `0x8` | 8 | |
| `0x2f` | 47 | `'/'` (juste avant `'0'`) |
| `0x39` | 57 | `'9'` |
| `0x30` | 48 | `'0'` |
| `0x7` | 7 | |
| `0x28` | 40 | `'('` (mais ici c'est sûrement le nombre 40) |

À ce stade, on peut déjà soupçonner que des chiffres sont en jeu (`'0'`, `'9'`).

## Chapitre 2

**2.1** `eax = 7`, `ebx = 7`, `eax = 10`, `eax = 70`, `eax = 70 - 16 = 54` (`0x36`). À la fin : `eax = 54`, `ebx = 7`.

**2.2** `eax = x` ; `eax = 2x` ; `eax = 3x` ; `eax = 12x`. Donc `x * 12`.

**2.3** 47 / 5 : quotient `eax = 9`, reste `edx = 2`.

**2.4** `eax = rdi + rdi*2 = rdi * 3`. Équivalent : `mov eax, edi` / `shl eax, 1` / `add eax, edi`.

**2.5** Avec gcc 13 en `-O2` :

- `f1` : `lea eax, [rdi*8]` (ou `shl`). Multiplier par une puissance de 2 = décaler.
- `f2` : `lea eax, [rdi+rdi*8]`. `x * 9 = x + 8x`.
- `f3` : `mov eax, edi` / `shr eax, 0x1f` / `add eax, edi` / `sar eax, 1`. `shr eax, 31` extrait le bit de signe (1 si négatif, 0 sinon). On ajoute 1 aux nombres négatifs avant de décaler, pour que `-3 / 2` donne `-1` (arrondi vers zéro, comme le veut le C) et non `-2` (ce que donnerait `sar` seul).

**2.6** Fichier pour l'exercice 2.1 (le principe est le même pour 2.3) :

```nasm
section .text
    global _start
_start:
    mov eax, 7
    mov ebx, eax
    add eax, 3
    imul eax, ebx
    sub eax, 0x10
    mov eax, 60
    xor edi, edi
    syscall
```

Dans GDB : `starti`, `si 5`, puis `p $eax` affiche `54` et `p $ebx` affiche `7`. Pour 2.3 : après les quatre instructions (`si 4`), `p $eax` affiche `9` et `p $edx` affiche `2`. Attention à ne pas exécuter `mov eax, 60` avant de lire `eax` : cette instruction écrase la valeur.

**Fil rouge**

1. `rdi` contient le **premier argument**. On le range sur 8 octets (`QWORD`) : c'est un `long` ou, plus probablement, un **pointeur**.
2. `[rbp-0x4]` est un `DWORD` (4 octets, un `int`), initialisé à 0. Nom provisoire : `compteur` (on verra que c'est une longueur).
3. `compteur++` (ou `compteur += 1`).
4. On lit quelque chose, on retire `0x30` (`'0'`), et on l'ajoute à `[rbp-0x8]`. Retirer `'0'` à un caractère chiffre donne sa valeur numérique : on **additionne des chiffres**. `[rbp-0x8]` est une **somme**.
5. Elle renvoie 0 ou 1 : un booléen (vrai / faux), probablement "code valide ou non".

## Chapitre 3

**3.1** a) pris (5 == 5). b) non pris (3 > 7 est faux). c) pris (-1 < 1 en signé). d) **non pris** : en non signé, -1 vaut `0xFFFFFFFF`, qui n'est pas en dessous de 1. e) non pris (eax vaut 0, donc ZF = 1, et `jne` saute si ZF = 0).

**3.2**

```c
if (a > b)        /* jle inversé */
    eax = a;
else
    eax = b;
```

C'est `max(a, b)`.

**3.3** `eax = (x > 0x41);` soit `return x > 'A';` (comparaison signée, `setg`).

**3.4** Exemple en `-O0` (à adapter au résultat obtenu) : deux `cmp` avec 11 et 17 (le compilateur transforme `age < 12` en `age <= 11` : `cmp ..., 0xb` / `jg`). En `-O2`, gcc peut produire des `cmov` ou des `setcc` au lieu des sauts.

**Fil rouge**

1. Si `[rbp-0x4]` (le compteur) n'est pas égal à 8, on met 0 dans `eax` et on saute à la fin : `if (compteur != 8) return 0;`.
2. `cmp al, 0x2f` / `jle refus` puis `cmp al, 0x39` / `jle suite` (sinon on tombe dans le refus). Donc on refuse si `c <= '/'` (c'est-à-dire `c < '0'`) ou si `c > '9'` : `if (c < '0' || c > '9') return 0;`. Le caractère doit être un chiffre.
3. On compare `[rbp-0x8]` (la somme) à `0x28` = 40 : `if (somme != 40) return 0;`.
4. On compare l'octet pointé par l'argument (`[rax]` avec `rax` = argument) et celui à `argument + 7` : le premier et le huitième caractère.
5. Quatre chemins mènent à `return 0` (longueur, chiffre, somme, premier et dernier caractère) et un seul à `return 1`, tout au bout. Toutes les conditions doivent être remplies : c'est une suite de vérifications, chacune pouvant rejeter le code.

## Chapitre 4

**4.1**

```c
int f(int n)
{
    int r = 1;
    while (n > 1) {
        r = r * n;
        n--;
    }
    return r;
}
```

C'est la factorielle. Pour 5 : 120.

**4.2** La boucle de parcours a la forme `movzx eax, BYTE PTR [...]` / `test al, al` / saut. Chaque voyelle donne un `cmp al, 0x61` (`'a'`), `0x65` (`'e'`), `0x69`, `0x6f`, `0x75`, `0x79`, souvent suivis de `je` vers le même bloc qui incrémente le compteur (c'est un `||`).

**4.3** La fonction additionne les codes ASCII des caractères jusqu'au 0 final. Pour `"abc"` : 97 + 98 + 99 = 294 (`0x126`).

**Fil rouge**

1. Deux sauts arrière : `401181: jne 40116b` et `4011f3: jle 4011a3`. Donc deux boucles.
2. Compteur `[rbp-0x4]`. On lit le caractère d'indice `[rbp-0x4]` et on continue tant qu'il n'est pas nul (`test al, al` / `jne`). C'est un **calcul de longueur** de chaîne (un `strlen` à la main). `[rbp-0x4]` = `len`.
3. ```c
   int len = 0;
   while (s[len] != 0)
       len++;
   if (len != 8)
       return 0;
   ```
4. Initialisation : `somme = 0` (`[rbp-0x8]`) et `i = 0` (`[rbp-0xc]`). Condition : `i <= 7` (donc `i < 8`). Incrément : `i++` en `4011eb`. 8 tours. `[rbp-0xc]` = `i`.
5. La sortie anticipée est le test "chiffre" (`4011cb: mov eax, 0` puis `jmp` vers la fin) : la fonction renvoie 0 dès qu'un caractère n'est pas un chiffre.

## Chapitre 5

**5.1** Deux arguments : un pointeur (64 bits, déréférencé `[rax]`) vers un `int` (lu en `DWORD`), et un `int`. Retour : `int`.

```c
int f(int *p, int n) { return *p + n; }
```

**5.2** `memcmp(buffer_local, "S3cr3t_K3y_2026!", 16) == 0` : le buffer local à `rbp-0x30` doit contenir exactement ces 16 octets. `je ok` : si `memcmp` renvoie 0 (égalité), succès.

**5.3** Dans `main`, on doit trouver : `mov edi, 0x7` / `call carre` (argument dans `edi`), puis `mov esi, eax` (le résultat devient le 2e argument de `printf`), `lea rdi, [rip+...]` (la chaîne de format `"%d\n"`, 1er argument), `mov eax, 0x0` (aucun registre vectoriel), `call printf`.

**5.4** `call` a empilé l'adresse de retour (rsp pointait dessus en entrant), puis `push rbp` a empilé 8 octets de plus : l'adresse de retour est en `[rsp+8]`.

**5.5** Au point d'arrêt `*carre` (avant le prologue), `x/gx $rsp` montre l'adresse de retour au sommet, et `info symbol` la situe dans `main`, juste après le `call`. Après `push rbp`, `rsp` a diminué de 8 : l'adresse de retour est passée de `[rsp]` à `[rsp+8]`, ce qui confirme la réponse de 5.4. Les adresses exactes varient d'une machine à l'autre, seule la relation compte.

**Fil rouge**

1. Un seul argument : seul `rdi` est utilisé avant d'être écrit (rangé en `[rbp-0x18]`). `rsi`, `rdx`... ne sont jamais lus.
2. On charge la valeur (8 octets), on y ajoute un indice, et on lit **un octet** à cette adresse : c'est un pointeur vers des `char`, donc `char *` (en fait `const char *` puisqu'on ne l'écrit jamais, mais l'assembleur ne permet pas de le savoir).
3. - `[rbp-0x4]` : `int len`
   - `[rbp-0x8]` : `int somme`
   - `[rbp-0xc]` : `int i`
   - `[rbp-0x18]` : `char *s` (l'argument)
4. Elle renvoie `eax` qui vaut 0 ou 1 : un `int` utilisé comme booléen.
5. `int verifier(const char *s);`
6. Dans `main`, on vérifie `argc == 2`, puis `mov rax, [rbp-0x10]` (argv) / `add rax, 0x8` / `mov rax, [rax]` : c'est `argv[1]`, passé dans `rdi`. Donc `verifier(argv[1])`. Le résultat est testé (`test eax, eax` / `je`) pour choisir entre "Code accepte" et "Code refuse".

## Chapitre 6

**6.1** a) `char` (ou `unsigned char`) : `s[i]`. b) `int` : `t[i]`. c) 8 octets à l'offset 8 : `p[1]` si `p` est un tableau de pointeurs / `long`, ou un champ d'une structure. d) un champ `int` à l'offset `0x20` d'une structure : `p->champ`. e) un `short` signé : `t[i]` avec `short *t`.

**6.2** `[rbp-0x18]` est un `int *`. La fonction fait la somme des 10 premiers éléments :

```c
int somme10(int *t)
{
    int s = 0;
    for (int i = 0; i <= 9; i++)
        s += t[i];
    return s;
}
```

**6.3** `x` en 0, `y` en 4, `nom` en 8 (8 octets, de 8 à 15), `id` en 16 (`0x10`). Ici l'alignement tombe juste. Avec `nom[5]`, `id` restera en 16 : le compilateur ajoute 3 octets de remplissage (padding) pour aligner le `long` sur 8.

**6.4** `0x5f67616c665f6e75` en little endian : `un_flag_`. `0x6e696d61` : `amin`. Avec le 0 final : `"un_flag_amin"`.

**Fil rouge**

1. On charge `len` (int), on l'étend en 64 bits (`movsxd`, donc `len` est signé), on charge `s`, on ajoute : `rax = s + len`, on lit l'octet : `s[len]`.
2. Avec l'indice `i` : deux fois `s[i]` pour le test "chiffre", une fois `s[i]` pour l'addition.
3. `movsx` = extension **signée** : les caractères sont des `char` signés (le type `char` par défaut sur x86). Le calcul fait en C est `somme += s[i] - '0'` avec `s[i]` promu en `int`.
4. `movzx edx, BYTE PTR [rax]` avec `rax = s` : `s[0]`. Puis `rax = s + 7` : `s[7]`. On compare le premier et le dernier caractère (le huitième).
5. Voir chapitre 7.

## Chapitre 7

**7.1** C'est `x / 2` pour un `int` signé (voir exercice 2.5).

**7.2** Arguments empilés de droite à gauche, donc dans l'ordre C : `MessageBoxA(NULL, "Bravo, le mot de passe est correct", "Bravo", 0);`. Le premier `push` est le dernier argument (`uType = 0`, `MB_OK`). Convention **stdcall** (celle de l'API Windows) : c'est `MessageBoxA` qui nettoie la pile, donc pas de `add esp, 0x10` après l'appel.

**7.3** Le compilateur soustrait `0x61` à la valeur (`sub eax, 0x61`), puis compare à 5 (`cmp eax, 0x5` / `ja default`), et utilise le résultat comme indice dans la table. Un `switch` sur `'a'..'f'` devient une table indexée de 0 à 5.

**7.4** En Python, pour reproduire les instructions sur 64 bits :

```python
def div10(x):
    rdx = (x * 0x66666667) >> 32     # imul + shr 32 (Python gère le signe comme sar)
    edx = rdx >> 2                   # sar edx, 2
    signe = -1 if x < 0 else 0       # sar eax, 31
    return edx - signe               # sub edx, eax
print(div10(1234), div10(-1234))     # 123 -123
```

**7.5** `rdi` et `rsi` désignent le même tampon. À chaque tour, `lodsb` lit un octet (`al = *rsi++`), `xor al, 0x5a` le déchiffre, `stosb` le réécrit au même endroit (`*rdi++ = al`), et `loop` répète 0x40 fois. C'est un **déchiffrement XOR en place** de 64 octets avec la clé `0x5a` :

```c
for (int i = 0; i < 0x40; i++)
    buf[i] ^= 0x5a;
```

**Fil rouge**

1. ```c
   int verifier(const char *s)
   {
       int len = 0;
       while (s[len] != 0)
           len++;
       if (len != 8)
           return 0;

       int somme = 0;
       for (int i = 0; i < 8; i++) {
           if (s[i] < '0' || s[i] > '9')
               return 0;
           somme += s[i] - '0';
       }
       if (somme != 40)
           return 0;
       if (s[0] != s[7])
           return 0;
       return 1;
   }
   ```
2. Un code valide est composé de **8 chiffres**, dont la **somme vaut 40**, et dont le **premier et le dernier sont identiques**.
3. Le plus simple : `55555555` (8 x 5 = 40, et le premier égale le dernier). Autre exemple : `94444429` (9 + 4 + 4 + 4 + 4 + 4 + 2 + 9 = 40). Il faut toujours vérifier en lançant le programme plutôt que de tête : `./mystere 94444429` doit afficher "Code accepte".
4. Il faut que les 6 chiffres du milieu fassent 40 - 18 = 22 :

   ```python
   from itertools import product
   print(sum(1 for t in product(range(10), repeat=6) if sum(t) == 22))   # 43917
   ```

   Il y en a 43 917.
5. Si la version écrite suit le même ordre, le désassemblage en `-O0` est identique instruction par instruction. De petites différences (un `while` au lieu d'un `for`, `<= 7` au lieu de `< 8`) peuvent donner exactement le même code : c'est normal, plusieurs codes C mènent au même assembleur. C'est d'ailleurs pour cette raison qu'un décompilateur ne retrouve jamais "le" code source original.
6. Dans la version `-O2` :
   - la première boucle est devenue un **appel à `strlen`** : le compilateur a reconnu la boucle ;
   - il appelle `strlen(s + 1)` (`lea rdi, [rdi+0x1]`) car il a déjà lu `s[0]` pour tester s'il est nul (`test bpl, bpl`). Donc `strlen(s + 1) == 7` équivaut à `strlen(s) == 8` ;
   - `movsx eax, [rdx]` / `lea esi, [rax-0x30]` / `cmp sil, 0x9` / `ja refus` : on calcule `c - '0'` et on le compare en **non signé** à 9. Si `c < '0'`, la soustraction donne un nombre négatif, donc énorme en non signé, donc `> 9`. Un seul test remplace `c < '0' || c > '9'` ;
   - `sete al` met `al` à 1 si `s[7] == s[0]`, 0 sinon : c'est le `return s[0] == s[7];` final, sans saut.

## Exercices de lecture

### Chapitre 2 {#lecture-02}

1. **2.L1** : le premier argument arrive dans `edi` et est recopié en `-0x4(%rbp)` ; le deuxième dans `esi`, recopié en `-0x8(%rbp)`.
2. **2.L2** : `mov %edx,%eax` copie `a` dans `eax` ; `add %eax,%eax` double `eax` (il vaut `2a`) ; `add %eax,%edx` ajoute ce `2a` au `a` déjà dans `edx`, qui vaut donc `3a`. On calcule `a * 3`.
3. **2.L3** : ensuite `eax` reçoit `b`, on ajoute `3a` (`eax = b + 3a`), puis `sub $0x7`. La fonction renvoie `a * 3 + b - 7`.

### Chapitre 3 {#lecture-03}

1. **3.L1** : les comparaisons sont les deux `cmpl $0x0,-0x4(%rbp)` (comparer `n` à 0). Les sauts sont `jns`, `jne` et les `jmp`. `jns` saute si le résultat n'est pas négatif, donc ici si `n >= 0`.
2. **3.L2** : `n = -5` : `jns` non pris (négatif), on fait `eax = 0xffffffff`, c'est-à-dire `-1`. `n = 0` : `jns` pris, puis `jne` non pris (égal à 0), `eax = 0`. `n = 7` : `jns` pris, `jne` pris, `eax = 1`.
3. **3.L3** :

   ```c
   int classer(int n)
   {
       if (n < 0) return -1;
       else if (n == 0) return 0;
       else return 1;
   }
   ```

### Chapitre 4 {#lecture-04}

1. **4.L1** : le saut arrière est `jle CORPS`, c'est la boucle. `-0x4` est l'accumulateur `s`, `-0x8` le compteur `i`, `-0x14` l'argument `n`.
2. **4.L2** : (A) `s = 0`, (B) `i = 1`, (C) `s += i`, (D) `i++`. Le code saute d'abord vers `TEST` pour évaluer la condition avant le premier tour : c'est la forme habituelle d'une boucle `for`, dont le test précède le corps (si `n < 1`, le corps n'est jamais exécuté).
3. **4.L3** :

   ```c
   int somme(int n)
   {
       int s = 0;
       for (int i = 1; i <= n; i++)
           s += i;
       return s;
   }
   ```

   `somme(5)` renvoie `1 + 2 + 3 + 4 + 5 = 15`.

### Chapitre 5 {#lecture-05}

1. **5.L1** : `push %rbp` / `mov %rsp,%rbp` installent le cadre de pile (`rbp` sert de base fixe pour désigner les variables locales et les arguments sauvés). `pop %rbp` restaure l'ancien `rbp`, et `ret` dépile l'adresse de retour et y saute.
2. **5.L2** : le premier argument est sauvé sur 8 octets (`mov %rdi,...`), donc un **pointeur** ; le deuxième sur 4 (`mov %esi,...`), donc un `int` ; le retour est dans `eax`, donc un `int`.
3. **5.L3** : un prototype plausible est `int element(int *t, int i);`. L'accès final (adresse `t + i*4`, puis lecture) correspond à `return t[i];`.

### Chapitre 6 {#lecture-06}

1. **6.L1** : le `4` est la taille d'un `int` (4 octets). Ensemble, (A) et (B) calculent `base + i * 4`, c'est-à-dire l'adresse de `t[i]`.
2. **6.L2** : (D) `movslq` étend l'indice de 32 bits signés vers 64 bits, pour le calcul d'adresse. (E) `movzbl (%rax),%eax` lit **un seul octet** à l'adresse `rax` et complète par des zéros : c'est un `char`. (F) `test %al,%al` suivi de `jne` continue tant que l'octet n'est pas nul, donc jusqu'au `'\0'`. On lit un octet car un `char` fait un octet.
3. **6.L3** :

   ```c
   int element(const int *t, int i)
   {
       return t[i];
   }
   ```

### Chapitre 7 {#lecture-07}

1. **7.L1** : le code compare `k` à 2 (`je CAS2`), puis `jg DEFAUT` si `k > 2` ; sinon il compare à 0 (`je CAS0`) et à 1 (`je CAS1`), et termine par `jmp DEFAUT`. C'est l'aiguillage du `switch`.
2. **7.L2** : `CAS0` fait `add` (`a + b`), `CAS1` fait `sub` (`a - b`), `CAS2` fait `imul` (`a * b`), `DEFAUT` met `eax` à 0.
3. **7.L3** :

   ```c
   int op(int a, int b, int k)
   {
       switch (k)
       {
           case 0: return a + b;
           case 1: return a - b;
           case 2: return a * b;
           default: return 0;
       }
   }
   ```

## Challenge de synthèse {#challenge-synthese}

**g1** : `edx = a + b`, `eax = a - b`, puis `imul` : la fonction renvoie `(a + b) * (a - b)`, c'est-à-dire `a*a - b*b`.

```c
int g1(int a, int b) { return (a + b) * (a - b); }
```

**g2** : accumulateur initialisé à 1, boucle qui multiplie par `n` puis décrémente `n` tant que `n > 1`. C'est la **factorielle**.

```c
int g2(int n)
{
    int r = 1;
    while (n > 1) { r = r * n; n = n - 1; }
    return r;
}
```

`g2(5)` renvoie `5 * 4 * 3 * 2 = 120`.

**g3** : `0x61` est le code ASCII de `'a'`. La fonction parcourt la chaîne jusqu'au `'\0'` et compte les caractères égaux à `'a'`.

```c
int g3(const char *s)
{
    int c = 0;
    for (int i = 0; s[i]; i++)
        if (s[i] == 'a')
            c++;
    return c;
}
```

## Convention d'appel en profondeur {#convention-appel}

Chaque solution se termine par `section .note.GNU-stack noalloc noexec nowrite progbits` pour éviter l'avertissement du linker.

**9.1** Le 7e argument est juste au-dessus de l'adresse de retour :

```nasm
septieme:
    mov rax, [rsp+8]
    ret
```

`septieme(1,2,3,4,5,6,777)` renvoie `777`. Les six premiers (`1`-`6`) sont dans `rdi`-`r9` et ne servent pas ; seul le septième, sur la pile, est lu.

**9.2** Les six premiers en registres, les deux derniers sur la pile :

```nasm
somme8:
    mov rax, rdi
    add rax, rsi
    add rax, rdx
    add rax, rcx
    add rax, r8
    add rax, r9
    add rax, [rsp+8]        ; 7e argument
    add rax, [rsp+16]       ; 8e argument
    ret
```

`somme8(1,2,3,4,5,6,7,8)` renvoie `36`.

**9.3** Les `double` vivent dans les registres `xmm` :

```nasm
section .data
    deux dq 2.0
section .text
moyenne:
    addsd xmm0, xmm1       ; a + b
    divsd xmm0, [rel deux] ; / 2
    ret
```

`moyenne(3.0, 8.0)` renvoie `5.50`. Le résultat est laissé dans `xmm0`, registre de retour des flottants.

**9.4** Version correcte :

```nasm
    lea rdi, [rel fmt]     ; "entier %d et flottant %.1f\n"
    mov esi, 42            ; %d
    movsd xmm0, [rel pi]   ; %f -> xmm0
    mov eax, 1             ; AL = 1 registre vectoriel
    call printf wrt ..plt
```

Avec `mov eax, 1`, on obtient `entier 42 et flottant 3.1`. Avec `al = 0`, `printf` croit qu'aucun `xmm` n'est utilisé, ne va pas lire `xmm0`, et affiche `entier 42 et flottant 0.0`. La règle variadique impose que `al` contienne le nombre de registres `xmm` employés.

**9.5** Les deux champs reviennent dans `rax` (premier champ) et `rdx` (second) :

```nasm
paire:
    mov rax, rdi           ; x = a
    lea rdx, [rsi+rsi]     ; y = 2*b
    ret
```

`paire(10, 21)` donne `x = 10`, `y = 42`. Une structure de 16 octets ou moins revient dans `rax:rdx`, sans pointeur caché.

**9.6** Fonction feuille utilisant la red zone :

```nasm
rouge:
    mov [rsp-8], rdi       ; red zone : écriture sous rsp sans réserver
    mov [rsp-16], rsi
    mov rax, [rsp-8]
    add rax, [rsp-16]
    ret
```

`rouge(40, 2)` renvoie `42`. C'est valide car `rouge` est une feuille : les 128 octets sous `rsp` lui sont garantis. Si elle appelait une autre fonction, le `call` pousserait l'adresse de retour à `[rsp-8]` et écraserait les données rangées dans la red zone : il faudrait alors réserver l'espace avec un vrai `sub rsp`.

## Structures et tableaux en assembleur {#structures-tableaux}

Chaque objet NASM se termine par `section .note.GNU-stack noalloc noexec nowrite progbits`.

**10.1** `y` est à l'offset 4 :

```nasm
lire_y:
    mov eax, [rdi+4]
    ret
```

**10.2** Modification en place via le pointeur (offset 0) :

```nasm
double_x:
    sal dword [rdi], 1     ; p->x <<= 1, soit *2
    ret
```

Après l'appel, le `x` de la structure côté C est bien doublé : on a écrit à travers le pointeur, pas sur une copie.

**10.3** `Enr` fait 16 octets (`id` à 0, padding, `val` à 8) :

```nasm
somme_val:
    xor eax, eax
    xor ecx, ecx
.b:
    cmp rcx, rsi
    jge .f
    mov rdx, rcx
    sal rdx, 4             ; i * 16
    add rax, [rdi+rdx+8]   ; t[i].val
    inc rcx
    jmp .b
.f:
    ret
```

Sur `{{1,100},{2,20},{3,3}}`, le résultat est `123`. L'erreur classique serait de multiplier l'index par 12 (la somme des tailles brutes) au lieu de 16 (la taille réelle avec padding).

**10.4** Index linéaire `i*ncol + j`, étendu en 64 bits :

```nasm
element:
    mov eax, edx          ; i
    imul eax, esi         ; i*ncol
    add eax, ecx          ; +j
    cdqe
    mov eax, [rdi+rax*4]
    ret
```

`element(&m[0][0], 3, 1, 2)` renvoie `6` (c'est `m[1][2]`).

**10.5** Le champ est un `unsigned char` : on étend par zéro avec `movzx`, sinon une valeur comme 200 (bit de poids fort à 1) deviendrait négative avec `movsx`.

```nasm
lire_flags:
    movzx eax, byte [rdi]
    ret
```

`flags = 200` renvoie bien `200`, et non `-56`.

**10.6** Le désassemblage donne tout :

- `movsxd rax, DWORD PTR [rdi]` : offset 0, taille 4 (`DWORD`), signé (`movsxd`) -> `int a`.
- `movzx edx, BYTE PTR [rdi+0x4]` : offset 4, taille 1 (`BYTE`), non signé (`movzx`) -> `unsigned char b`.
- `add rax, QWORD PTR [rdi+0x8]` : offset 8, taille 8 (`QWORD`) -> `long c`.

Déclaration compatible : `struct { int a; unsigned char b; long c; };`, de taille 16 octets (padding entre `b` et `c`). Les tailles d'accès et les extensions suffisent à retrouver les types sans la source.
