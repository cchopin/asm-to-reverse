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
