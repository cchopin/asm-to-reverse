# Solutions de la partie 2

Les adresses citées viennent de binaires compilés avec gcc 13 (Ubuntu 24.04). Avec un autre compilateur, elles peuvent différer : c'est la démarche qui compte. Dans le conteneur du cours (gcc 13), elles doivent être identiques. Les scripts sont dans le dossier `solutions/scripts` : [keygen_v3.py](scripts/keygen_v3.py), [solve_v2.py](scripts/solve_v2.py), [decode_v4.py](scripts/decode_v4.py), [v4.gdb](scripts/v4.gdb), [decode_v5.py](scripts/decode_v5.py), [coffre_v5.yar](scripts/coffre_v5.yar).

## Exercices des chapitres

### Chapitre 8

**8.1** `/bin/ls` : ELF 64 bits, PIE, dynamiquement lié, **strippé** (`nm` répond "no symbols"). `strings` montre les messages d'aide et les options ; `nm -D` montre les fonctions utilisées (`opendir`, `readdir`, `stat`...), ce qui suffit à deviner qu'il liste des fichiers.

**8.2** `strings` ne trouve pas `1337` : c'est une constante numérique dans le code (`cmp eax, 0x539`), pas une chaîne. `ltrace` montre `scanf("%d", ...)` mais pas la comparaison (faite par le programme, pas par une fonction de bibliothèque). `objdump` montre le `cmp ..., 0x539` : `0x539` = 1337. Leçon : chaque outil a son angle mort.

### Chapitres 9 et 10

**9.1 et 9.2** Exercices d'observation, pas de réponse unique. En général, Ghidra retrouve bien la structure et les types simples, mais choisit de mauvais noms et parfois des types trop larges (`ulong` au lieu de `int`). En `-O2`, il affiche souvent le test des chiffres sous la forme `(byte)(c - 0x30) < 10`, qui est la traduction directe du `sub` / `cmp` / `ja`.

**10.1** À la première comparaison (`cmp DWORD PTR [rbp-0x4],0x8`), `[rbp-0x4]` vaut 8 : la boucle de longueur a compté les 8 caractères de `12345678`.

**10.2** `break verifier`, `run 11111111`, `finish` (GDB s'arrête juste après le `call verifier` dans `main`, avec la valeur de retour dans `eax`), `set $eax = 1`, `continue` : "Code accepte".

**10.3**

```
set pagination off
break *0x4011ef
commands
  silent
  printf "i=%d somme=%d\n", *(int*)($rbp-0xc), *(int*)($rbp-0x8)
  continue
end
run 12345678
```

(`0x4011ef` est le test de la deuxième boucle, atteint à chaque tour. Le binaire est compilé avec `-no-pie`, donc les adresses du listing sont les vraies.)

### Chapitre 11

**11.1** a) transformation puis comparaison (attention, chaque octet dépend du suivant : il faut résoudre en chaîne, ou avec z3) ; b) comparaison d'empreinte (force brute si l'espace est petit, sinon chercher une faiblesse) ; c) contraintes, z3 ; d) machine virtuelle, écrire un désassembleur du bytecode.

**11.2** La solution est `z3ok`. Pour chercher une autre solution : `solveur.add(Or([c != m[c] for c in s]))` puis `check()`. Ici, la réponse est `unsat` : la solution est unique (chaque contrainte fixe un caractère à partir du suivant).

**11.3** Dans `main`, le `je` qui suit `test eax, eax` après `call verifier` peut devenir deux `nop` (on tombe toujours sur "Code accepte"). Dans `verifier`, on peut remplacer les premières instructions par `mov eax, 1` / `ret` (`B8 01 00 00 00 C3`).

### Chapitre 12

**12.1** à **12.3** : voir la solution du coffre v4, les techniques sont les mêmes. Pour `/proc/self/status`, mettre un point d'arrêt après la lecture et modifier la valeur lue, ou patcher la comparaison.

### Chapitre 13

**13.1** Les valeurs dépendent de la version de `putty.exe`. On doit trouver des sections classiques (`.text`, `.rdata`, `.data`, `.pdata`, `.rsrc`, `.reloc`), une entropie modérée (autour de 6 pour `.text`, ce qui indique un binaire non packé), et des imports de `KERNEL32.dll`, `USER32.dll`, `GDI32.dll`, `ADVAPI32.dll` (registre, pour mémoriser les sessions) et de bibliothèques réseau. capa signale par exemple des capacités réseau (connexion TCP), d'accès au registre et de chiffrement : un logiciel légitime peut avoir des capacités "suspectes", c'est leur **combinaison** et leur **contexte** qui comptent.

**13.2** a) rançongiciel (parcours de fichiers, lecture, chiffrement, réécriture, suppression des originaux) ; b) injection de code dans un autre processus ; c) keylogger qui envoie les frappes par HTTP ; d) programme packé ou chargeur (loader) : il alloue de la mémoire, y écrit du code, la rend exécutable (`VirtualProtect`) et résout ses imports à la main.

**13.3** Exemple de règle, à partir des chaînes propres à chaque version :

```
rule Coffre_v1_v2
{
    strings:
        $banniere = "=== Coffre v" ascii
        $refus = "Acces refuse." ascii
    condition:
        uint32(0) == 0x464c457f and $banniere and $refus
}
```

`coffre_v3` affiche "Serial invalide." et non "Acces refuse." : il n'est pas détecté. `/bin/ls` non plus.

**13.4** Exercice ouvert. Points d'attention : distinguer dans le rapport ce qui relève du malware (processus enfants, fichiers déposés, connexions) de l'activité normale du système, et relever en priorité les artefacts stables (mutex, chemins, user-agent) plutôt que les seules adresses IP.

## Coffre v1

1. `file` : ELF 64 bits, PIE, non strippé. `nm -D` : `fgets`, `strcmp`, `strcspn`, `puts`, `printf`. Hypothèse : comparaison directe de la saisie avec une chaîne fixe par `strcmp`.
2. `ltrace` : `strcmp("test", "Sesame_ouvre_toi")`.
3. Dans `main` : `lea rsi, [rip+...]` charge l'adresse de `"Sesame_ouvre_toi"`, `rdi` reçoit la saisie, `call strcmp`, `test eax, eax`, `jne` vers "Acces refuse.".
4. Mot de passe : `Sesame_ouvre_toi`. Flag : `FLAG{v1_strings_suffit}`.
5. Trois façons : `strings` (la chaîne est en clair, et le flag aussi !), `ltrace`, point d'arrêt sur `strcmp` dans GDB (`x/s $rsi`), ou lecture du désassemblage. Si le mot de passe n'était pas en clair mais calculé juste avant le `strcmp`, ltrace et GDB auraient encore marché, pas `strings`. Si la comparaison était faite à la main, seul GDB (ou la lecture du code) marcherait.

## Coffre v2

**En statique (chapitre 9)**

1. `strings` ne montre pas le mot de passe. `nm -D` : `strlen` mais **pas** de `strcmp` ni `memcmp` : la comparaison est faite à la main.
2. `ltrace` montre `strlen`, rien d'autre d'utile.
3. Dans Ghidra, depuis la chaîne `"Coffre ouvert. FLAG{%s}\n"`, on remonte au `if (verifier(saisie))` dans `main`. `verifier` : la longueur doit être 19 (`0x13`), et pour chaque `i`, `saisie[i] ^ 0x2a` doit être égal à `cle[i]`. En une phrase : **chaque caractère, XORé avec 0x2A, doit être égal à l'octet correspondant d'un tableau global**.
4. Le tableau `cle` (19 octets) : `78 19 5c 19 58 59 19 75 47 19 75 1b 4c 75 5f 75 49 1e 44`.
5. Comme `(s ^ 0x2a) == cle` équivaut à `s == cle ^ 0x2a` : voir [solve_v2.py](scripts/solve_v2.py). Mot de passe : `R3v3rs3_m3_1f_u_c4n`. Flag : `FLAG{R3v3rs3_m3_1f_u_c4n}`.

**En dynamique (chapitre 10)**

1. `disassemble verifier` : `cmp dl,al` en `verifier+87` (offset `0x1240`).
2. Fichier d'entrée : 19 caractères, `AAAAAAAAAAAAAAAAAAA`.
3. Premier arrêt : `dl = 0x6b`, `al = 0x78`. `0x41 ^ 0x2a = 0x6b` : la transformation est un XOR avec `0x2a` (pour trancher avec une addition, `0x41 + 0x2a = 0x6b` aussi ! Il faut tester avec une autre lettre : `B` donne `0x68` avec le XOR et `0x6c` avec l'addition). `al` est la valeur attendue.
4. Script :

   ```
   set pagination off
   break *verifier+87
   commands
     silent
     printf "dl=%02x al=%02x\n", $dl, $al
     set $dl = $al
     continue
   end
   run < entree.txt
   ```

   Il affiche les 19 valeurs de `al` : `78 19 5c 19 ...`, et le coffre s'ouvre même (avec un faux flag, puisque le flag affiché est la saisie).
5. Valeurs XOR `0x2a` : `R3v3rs3_m3_1f_u_c4n`.
6. Ici, les deux sont rapides. Le dynamique est meilleur quand l'algorithme est compliqué mais que la valeur attendue apparaît en clair au moment de la comparaison ; le statique est meilleur quand le programme est protégé contre le débogage ou quand il faut comprendre l'algorithme de toute façon (keygen).

## Coffre v3

1. Le programme demande un nom et un serial. Le nom doit faire au moins 4 caractères (`cmp rax, 0x3` / `jbe` après `strlen` : "Nom trop court."). `nm -D` montre `strtoul`, et dans le désassemblage on voit `mov edx, 0x10` juste avant l'appel : base 16, le serial est en **hexadécimal**.
2. La boucle inlinée dans `main` (gcc 13, `-O1`) :

   ```
   12f8: mov    eax,0x1337            ; h = 0x1337
   ...
   1301: mov    ecx,eax
   1303: shl    ecx,0x5               ; ecx = h * 32
   1306: sub    ecx,eax               ; ecx = h * 32 - h = h * 31
   1308: movzx  edx,dl                ; c = caractère courant
   130b: lea    eax,[rdx+rcx*1]       ; h = h * 31 + c
   130e: rol    eax,0x3               ; h = rotation à gauche de 3 bits
   1311: add    rsi,0x1               ; caractère suivant
   1315: movzx  edx,BYTE PTR [rsi-0x1]
   1319: test   dl,dl
   131b: jne    1301                  ; jusqu'au 0 final
   131d: xor    eax,0xc0ffee          ; h ^= 0xC0FFEE
   ...
   1333: call   strtoul               ; serial converti depuis l'hexadécimal
   1338: cmp    ebx,eax               ; comparaison
   ```

   `shl` puis `sub` = multiplication par 31 (chapitre 7). `rol eax, 3` = rotation à gauche de 3 bits sur 32 bits.
3. Valeur initiale `0x1337`. Après la boucle : XOR avec `0xC0FFEE`.
4. Voir [keygen_v3.py](scripts/keygen_v3.py). Les `& 0xFFFFFFFF` sont indispensables.
5. `python3 keygen_v3.py Clemence` donne `F6E88EDC`, et le coffre affiche `FLAG{keygen_f6e88edc}`.
6. Oui : patcher le `je` en `133a` (juste après le `cmp ebx, eax`) en `jmp` ouvre le coffre. Le flag affiché dépend du nom (c'est la valeur calculée), donc il reste correct pour ce nom ! Ici, le patch suffit. Ce n'est pas le cas quand le flag dépend du serial.

## Coffre v4

1. `file` : strippé. `nm -D` : `ptrace` (anti-debug), `clock_gettime` (mesure du temps), `puts`. `strings` : "Je te vois. Le coffre reste ferme." et le message final.
2. Lancé normalement, il affiche le message final. Dans GDB, il affiche "Je te vois.".
3. Dans `entry` (ou `_start`), le `lea rdi, [rip+...]` avant l'appel à `__libc_start_main` donne `main` (offset `0x11c9` ici).
4. Protection 1 : `call ptrace(0, 0, 0, 0)` (`PTRACE_TRACEME`), puis `cmp rax, -1` / `je echec`. Protection 2 : deux `clock_gettime` autour d'une boucle de 1 000 000 tours ; on calcule les millisecondes écoulées (`imul rcx, rcx, 0x3e8` = secondes x 1000, et `idiv` par 1 000 000 pour les nanosecondes) et `cmp rcx, 0x1f4` (500 ms) / `jle suite`. Sans pas à pas dans cette zone, elle ne se déclenche pas : on peut l'ignorer.
5. La fonction appelée avec le tampon et `0x17` (23) : `xor BYTE PTR [rax], dl` puis `lea edx, [rdx+rdx*4+0x3]`. Clé initiale `0x59`, et à chaque octet `k = k * 5 + 3` (sur 8 bits). C'est un XOR avec une clé qui évolue.
6. Méthode dynamique : voir [v4.gdb](scripts/v4.gdb). Point d'arrêt après `call ptrace`, `set $rax = 0`, point d'arrêt après le déchiffrement, `x/s $rbx`. Flag : `FLAG{anti_debug_vaincu}`.
7. Méthode statique : les 23 octets sont écrits sur la pile par trois `movabs` / `mov QWORD PTR`, le troisième en `[rsp+0x3f]`, qui **chevauche** le deuxième d'un octet (23 octets = 8 + 8 + 7, le compilateur écrit 8 octets à partir de l'octet 15). Voir [decode_v4.py](scripts/decode_v4.py).
8. Bonus : remplacer `rep stos` (`F3 AB`, en `0x12e9`) par `90 90`, et le `lea rdi, [rip+0xd3e]` (7 octets, en `0x12eb`) par `mov rdi, rbx` (`48 89 DF`) suivi de 4 `nop`. Le `puts` affiche alors le flag. Il faut aussi contourner `ptrace`... mais seulement sous GDB : lancé normalement, le binaire patché affiche directement le flag.

## Coffre v5

### Démarche

1. SHA-256 à calculer sur son propre binaire (il dépend du compilateur). Strippé.
2. Imports : `sleep` (attente), `gethostname`, `getuid`, `getpwuid`, `sysconf` (reconnaissance et détection d'environnement), `strstr` (recherche dans une chaîne), `open`, `fopen`, `fwrite`, `dprintf`, `fprintf` (écriture de fichiers), `snprintf` (construction d'une chaîne), `getpid`. Aucun import réseau. Les chaînes bizarres (`TXQQERhA`, `DVYSUXO7`, `tvzgvpyr`...) sont des **chaînes chiffrées** : un XOR avec `0x37` transforme les minuscules en majuscules et symboles imprimables, d'où leur aspect de "presque texte".
3. Il dort 3 secondes (`clock_nanosleep` dans strace) avant de faire quoi que ce soit : une astuce contre les sandbox automatiques qui n'observent qu'un temps limité. Il crée `/tmp/.coffre_v5.lock` (avec son PID), lit `/etc/passwd` (via `getpwuid`), crée `/tmp/.coffre_v5.cache` et `/tmp/.coffre_v5_autostart.desktop`. La seconde fois, `open(..., O_CREAT|O_EXCL)` échoue sur le verrou (`EEXIST`) et le programme s'arrête : c'est un **verrou d'instance unique**, l'équivalent d'un mutex Windows. S'il ne fait rien dès la première fois, la machine a sans doute **un seul processeur**, ou un nom d'hôte contenant "sandbox" (voir 7).
4. Fonction de déchiffrement : petite boucle `movzx` / `xor ecx, 0x37` / `mov [rdi], cl` / `cmp dl, 0x37` / `jne`, appelée avant chaque usage d'une chaîne. Algorithme : XOR de chaque octet avec `0x37`, jusqu'à obtenir un octet nul (l'octet chiffré `0x37`).
5. Voir [decode_v5.py](scripts/decode_v5.py).
6. L'encodeur : chaque octet est XORé avec une clé qui commence à `0x42` et augmente de 1, puis écrit en hexadécimal (table `"0123456789abcdef"`). `decode_v5.py /tmp/.coffre_v5.cache` donne la **balise** : `id=CAMPAGNE-ROUGE-07&host=...&user=...&ua=Mozilla/5.0 (CoffreBot/1.0)&dst=http://c2.coffre.invalid:8443/gate.php`. Elle serait envoyée au serveur de commande.
7. Techniques :
   - anti-sandbox : `sleep(3)` au début ; arrêt si le nom d'hôte contient `sandbox` ; arrêt si moins de 2 processeurs (`sysconf(_SC_NPROCESSORS_ONLN)`) ;
   - instance unique : fichier verrou créé avec `O_EXCL` ;
   - reconnaissance : nom d'hôte, nom d'utilisateur ;
   - préparation de l'exfiltration : balise encodée et stockée ;
   - persistance : fichier `.desktop` avec `Hidden=true`. Un vrai malware l'écrirait dans `~/.config/autostart/` pour être lancé à chaque ouverture de session graphique. Ici il est écrit dans `/tmp` pour rester inoffensif, mais le **nom** (`autostart`) et le **contenu** trahissent l'intention.

### Exemple de rapport

> **Résumé.** `coffre_v5` est un implant Linux de petite taille. Il retarde son exécution, évite les environnements d'analyse, s'assure de n'être lancé qu'une fois, collecte le nom de la machine et de l'utilisateur, prépare une balise destinée à un serveur de commande et installe un fichier de démarrage automatique. Cet échantillon ne contient pas de code réseau : l'envoi est probablement délégué à un autre composant, ou l'échantillon est incomplet. Risque : modéré (reconnaissance et persistance).
>
> **Identification.** ELF 64 bits x86-64, PIE, strippé, dynamiquement lié à la glibc. Pas de packer. SHA-256 : (à compléter).
>
> **Comportement.**
> 1. Attente de 3 secondes.
> 2. Fin silencieuse si le nom d'hôte contient "sandbox" ou si la machine a moins de 2 processeurs.
> 3. Création du verrou `/tmp/.coffre_v5.lock` (contient le PID) ; fin silencieuse s'il existe déjà.
> 4. Collecte du nom d'hôte et du nom d'utilisateur.
> 5. Construction de la balise `id=<campagne>&host=<hôte>&user=<utilisateur>&ua=<user-agent>&dst=<C2>`, encodée (XOR à clé incrémentale à partir de 0x42, puis hexadécimal) et écrite dans `/tmp/.coffre_v5.cache`.
> 6. Écriture d'un fichier `.desktop` caché qui relance `/tmp/coffre_v5`.
>
> Les chaînes de configuration sont chiffrées par XOR avec la clé `0x37`.
>
> **Indicateurs.**
>
> | Type | Valeur |
> |---|---|
> | URL (C2) | `http://c2.coffre.invalid:8443/gate.php` |
> | Domaine | `c2.coffre.invalid` |
> | Port | 8443 |
> | User-Agent | `Mozilla/5.0 (CoffreBot/1.0)` |
> | Identifiant de campagne | `CAMPAGNE-ROUGE-07` |
> | Fichier | `/tmp/.coffre_v5.lock` |
> | Fichier | `/tmp/.coffre_v5.cache` |
> | Fichier (persistance) | `/tmp/.coffre_v5_autostart.desktop` (un vrai implant viserait `~/.config/autostart/`) |
> | Chemin exécuté | `/tmp/coffre_v5` |
> | Hash | SHA-256 de l'échantillon |
>
> **Détection.** Règle YARA : [coffre_v5.yar](scripts/coffre_v5.yar). Elle s'appuie sur la chaîne de format de la balise, la routine de déchiffrement (`xor reg, 0x37` suivi de `cmp reg8, 0x37`) et des éléments du fichier de persistance, et non sur l'URL, qui changerait d'une campagne à l'autre. Côté réseau : rechercher le user-agent `CoffreBot` et les requêtes vers `/gate.php`.

### La règle YARA

```
rule Coffre_v5_implant
{
    strings:
        $fmt  = "id=%s&host=%s&user=%s&ua=%s&dst=%s" ascii
        $desk = "Hidden=true" ascii
        $dec  = { 83 F? 37 [2-3] 80 F? 37 }
        $hex  = "0123456789abcdef" ascii
    condition:
        uint32(0) == 0x464c457f and filesize < 200KB and
        $fmt and $dec and ($desk or $hex)
}
```

`F?` signifie "un octet dont le premier chiffre hexa est F" (le second dépend du registre utilisé), et `[2-3]` "entre 2 et 3 octets quelconques". Testée : elle détecte `coffre_v5` mais ni les autres coffres, ni `/bin/ls`. Si le compilateur a produit une routine différente, il faut adapter `$dec` aux octets obtenus (`objdump -d` les affiche).

Nettoyage final : `rm -f /tmp/.coffre_v5*` (ou sortie du conteneur).

## Exercice de pont : votre propre coffre0 {#pont-coffre0}

1. `strings coffre0` montre `Coffre ouvert`, `Refuse`, `Usage : ./coffre0 <cle>`. Ils sont passés à `my_putstr` depuis `main`.
2. Dans `main`, les huit octets `43 6d 67 70 69 73 69 6c` sont chargés comme valeurs immédiates sur la pile (l'initialisation de `struct coffre c = {{...}}`), ou lues depuis la section de données selon l'optimisation.
3. La boucle compare, pour chaque `i`, `(cle[i] + i)` à `cible[i]`. Le `+ i` apparaît comme un `add` de l'indice de boucle au caractère chargé (souvent `add %edx,%eax` où `edx` est l'indice), juste avant le `cmp`.
4. La clé se déduit en calculant `cible[i] - i` :

   ```python
   cible = [0x43,0x6d,0x67,0x70,0x69,0x73,0x69,0x6c]
   print("".join(chr(c - i) for i, c in enumerate(cible)))   # Clemence
   ```

   C'est votre premier keygen : vous avez reconstruit la clé sans la connaître, à partir des seules données du binaire.

## CTF maison {#ctf-maison}

**Défi 1 (crack01).** `strings crack01` laisse apparaître `sesame`. Le programme fait un `strcmp` direct. Mot de passe : `sesame`.

**Défi 2 (crack02).** Chaque caractère saisi est comparé, après un `xor 0x5A`, à un tableau d'octets. On inverse en appliquant le même XOR aux octets stockés :

```python
stored = [0x37,0x33,0x28,0x35,0x33,0x28]
print("".join(chr(b ^ 0x5A) for b in stored))   # miroir
```

**Défi 3 (crack03).** La transformation dépend de la position : `cle[i] ^ (i*7)` est comparé au tableau. On inverse position par position :

```python
stored = [0x72,0x62,0x78,0x70,0x6e,0x50,0x4f]
print("".join(chr(b ^ (i * 7)) for i, b in enumerate(stored)))   # reverse
```

**Défi 4 (crack04).** Le serial vaut `(somme des codes ASCII du nom) * 1337 % 100000`. Keygen :

```python
def serial(nom):
    return (sum(ord(c) for c in nom) * 1337) % 100000
print(serial("clemence"))   # 7036
```

`./crack04 clemence 7036` affiche `Licence valide`. Le keygen marche pour n'importe quel nom.

**Défi 5 (crack05).** Les contraintes lues dans le code : `d0+d1+d2+d3 == 20`, `d0 == d3+1`, `d1 == d2*2`, `d0 > d1`. Avec z3, ou à la main : de `d1 = 2*d2` et `d0 = d3+1`, la somme donne `2*d3 + 3*d2 = 19` ; seul `d2 = 1, d3 = 8` respecte aussi `d0 > d1`, d'où `d0 = 9, d1 = 2`. Code unique : `9218`.

```python
from z3 import Int, Solver, sat
d = [Int(f"d{i}") for i in range(4)]
s = Solver()
for x in d: s.add(x >= 0, x <= 9)
s.add(sum(d) == 20, d[0] == d[3] + 1, d[1] == d[2] * 2, d[0] > d[1])
if s.check() == sat:
    m = s.model(); print("".join(str(m[x]) for x in d))   # 9218
```

**Défi 6 (crack06).** Analyse statique (Ghidra ou `strings`) : le mot de passe comparé est `antidebug`. Le programme appelle `ptrace(PTRACE_TRACEME, ...)` au début ; sous un débogueur, cet appel échoue (renvoie -1) et le programme s'arrête, car un processus déjà tracé ne peut pas se tracer lui-même.

Trois façons de contourner l'anti-debug pour l'observer en dynamique :

- lancer simplement `./crack06 antidebug` (sans débogueur) : affiche `Bravo` ;
- neutraliser `ptrace` avec un stub chargé par `LD_PRELOAD` :

  ```c
  /* fakeptrace.c : gcc -shared -fPIC -o fakeptrace.so fakeptrace.c */
  long ptrace(int r, int p, void *a, void *d) { (void)r;(void)p;(void)a;(void)d; return 0; }
  ```

  puis `LD_PRELOAD=./fakeptrace.so gdb ./crack06` ;
- ou patcher le saut conditionnel qui suit le test de la valeur de retour de `ptrace`, pour qu'il ne parte jamais dans la branche « debugger detecte » (voir le patching, chapitre 11).

## Laboratoire malware {#labo-malware}

### Cas 1 : la balise

Désobscurcissement : un simple `xor 0x3C`. Les chaînes cachées se décodent ainsi :

```python
dom  = [0x4f,0x45,0x52,0x5f,0x12,0x59,0x44,0x5d,0x51,0x4c,0x50,0x59,0x12,0x55,0x52,0x4a,0x5d,0x50,0x55,0x58]
camp = [0x7f,0x7d,0x71,0x6c,0x11,0x0e,0x0c,0x0e,0x0a,0x11,0x72,0x73,0x6a]
print("".join(chr(b ^ 0x3C) for b in dom))   # sync.example.invalid
print("".join(chr(b ^ 0x3C) for b in camp))  # CAMP-2026-NOV
```

Sous `strace`, on observe l'ouverture et l'écriture de `/tmp/.balise.lock`, puis un `connect` vers `127.0.0.1:9999` (qui échoue, rien n'écoute).

IOC :

| Type | Valeur |
| --- | --- |
| Fichier déposé | `/tmp/.balise.lock` (contient l'identifiant de campagne) |
| Domaine de contact | `sync.example.invalid` |
| Adresse / port | `127.0.0.1` port `9999` |
| Identifiant de campagne | `CAMP-2026-NOV` |
| Clé d'obscurcissement | XOR `0x3C` |

### Cas 2 : le dropper

`strings` ne montre pas la charge car elle est stockée chiffrée (un tableau d'octets) et déchiffrée seulement en mémoire, par un `xor 0x11`. On la retrouve sans exécuter le programme :

```python
payload = [0x72,0x79,0x70,0x63,0x76,0x74,0x31,0x77,0x70,0x72,0x65,0x78,0x72,0x74,0x31,
           0x57,0x5d,0x50,0x56,0x6a,0x75,0x63,0x7e,0x61,0x61,0x74,0x63,0x4e,0x75,0x74,0x72,0x7e,0x75,0x74,0x6c]
print("".join(chr(b ^ 0x11) for b in payload))   # charge factice FLAG{dropper_decode}
```

### Rapport type (cas 1)

| Rubrique | Contenu |
| --- | --- |
| Échantillon | `balise`, binaire ELF 64 bits (`file`), empreinte via `sha256sum balise` |
| Résumé | Au lancement, l'échantillon dépose un marqueur de persistance, puis tente de contacter un serveur distant en lui transmettant un identifiant de campagne. Adresse et campagne sont obscurcies par XOR. |
| Comportements | Persistance (fichier `/tmp/.balise.lock`), réseau (`connect` localhost:9999), obscurcissement XOR 0x3C des chaînes sensibles |
| IOC | voir le tableau ci-dessus |
| Recommandations | Détecter la présence du fichier marqueur et les connexions vers le domaine ; en environnement réel, bloquer le domaine et analyser le trafic sortant |

## Packing et UPX {#packing-upx}

**17.1** Sur un binaire ELF normal, l'entropie est modérée (souvent 1 à 6 bits/octet selon le remplissage), tandis que sa version compressée monte à environ 7.8-7.9. Mesures typiques : binaire normal `1.47`, compressé `7.86`, aléatoire `7.99`. Le seuil pratique est **~7.2 bits/octet** : au-dessus, le contenu est compressé ou chiffré.

**17.2** Après `gcc -O2 -s packed.c -o packed -lz`, `./packed` affiche son message, mais `strings packed | grep -i flag` ne renvoie rien. Le message n'est pas stocké en clair : il fait partie du payload compressé (`blob`), et n'est reconstitué qu'à l'exécution, par `uncompress` dans une zone `mmap`, juste avant le saut dessus. Tant qu'on n'exécute pas, le fichier ne le contient que sous forme compressée, illisible par `strings`.

**17.3** Le binaire packé présente typiquement : un point d'entrée déplacé (vers le stub), une grosse zone de données à forte entropie, peu de chaînes utiles, et pour un vrai UPX des sections nommées `UPX0`/`UPX1`. Sur un échantillon inconnu, la combinaison « entropie élevée + strings vide + point d'entrée hors `.text` habituelle + sections inhabituelles » est une signature de packing. (Le packer de démonstration garde des sections normales, puisqu'il s'agit d'un simple programme C embarquant un blob ; un vrai UPX modifie l'en-tête et les sections.)

**17.4** Dépaquetage statique, sans exécution :

```python
import zlib
blob = bytes(...)                 # octets compressés relevés dans la section de données
code = zlib.decompress(blob)      # décompression hors ligne
open("depaque.bin", "wb").write(code)
```

Puis `objdump -D -b binary -m i386:x86-64 -M intel depaque.bin` montre le shellcode original (`jmp`/`call`/`pop` puis `write` et `ret`), et `strings depaque.bin` révèle `code original execute: FLAG{depack_ok}`. Le binaire n'a jamais été lancé : c'est la méthode à privilégier face à un échantillon hostile, quand on connaît (ou devine) l'algorithme de compression.

**17.5** Dépaquetage dynamique générique : en bac à sable, lancer le binaire sous GDB, laisser le stub décompresser, et poser un point d'arrêt sur le **saut vers l'OEP** (le `jmp`/`call` vers l'adresse de la zone fraîchement rendue exécutable, souvent repérable par un `jmp` lointain ou un appel sur registre après un `mprotect`). À cet instant, le code original est en clair en mémoire : on le *dumpe* (`dump binary memory oep.bin debut fin`) et on l'analyse ensuite statiquement. Cette méthode marche pour **n'importe quel** packer, car tout packer finit par dévoiler le code en mémoire pour l'exécuter. Elle est plus générale que `upx -d`, qui ne fonctionne que pour UPX standard non modifié.

**17.6** Le binaire à **7.8** bits/octet est le plus probablement packé : cette valeur, proche du maximum de 8, indique un contenu compressé ou chiffré. 4.9 correspond à du code/texte ordinaire, et 6.1 à un binaire mixte non packé. Le seuil d'alerte est autour de 7.2.
