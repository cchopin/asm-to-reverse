# Solutions du capstone

À ne consulter qu'après avoir cherché. Les sources des cibles sont dans `capstone/ctf` et `capstone/malware` ; les scripts cités sont dans le dossier `solutions/scripts`.

## Défis CTF {#defis-ctf}

### Défi 1 : serrure

Dans `objdump -d -M intel serrure`, la fonction de dérivation est une boucle sur les caractères du nom. On y lit : une valeur initiale `0x5EED`, par caractère un `xor` avec l'octet, une rotation à gauche de 4 bits (`rol ...,4` ou la paire `shl`/`shr`+`or`), une addition de `0x9E37 + i`, et en sortie un masque `and 0xFFFFFF` (24 bits, soit 6 chiffres hexadécimaux).

Le keygen rejoue exactement ce calcul ([scripts/keygen_serrure.py](scripts/keygen_serrure.py)) :

```python
def melange(nom):
    h = 0x5EED
    for i, c in enumerate(nom.encode()):
        h ^= c
        h = ((h << 4) | (h >> 28)) & 0xFFFFFFFF   # rol 4 sur 32 bits
        h = (h + 0x9E37 + i) & 0xFFFFFFFF
    return h & 0xFFFFFF
```

Pour `Clemence`, le serial est `AEDA7E`, donc :

```
$ ./serrure Clemence AEDA7E
FLAG{serrure_AEDA7E}
```

Le piège classique est l'arithmétique sur 32 bits : il faut masquer à chaque étape (`& 0xFFFFFFFF`) pour reproduire le comportement d'un `unsigned int`, et bien faire une rotation (`rol`) et non un simple décalage.

### Défi 2 : machine

Le binaire embarque une machine virtuelle. Dans le désassemblage, on repère une boucle de *dispatch* : lecture d'un octet d'opcode, puis une série de `cmp` avec de petites constantes (`0x01`, `0x02`, `0x03`, `0x04`) dirigeant chacune vers un traitement. Le jeu d'instructions se lit ainsi :

| Opcode | Effet |
|---|---|
| `0x01 k` | `R += k` |
| `0x02 k` | `R ^= k` |
| `0x03 k` | `R = rol8(R, k)` |
| `0x04 k` | `R -= k` |
| `0xFF` | fin du programme, comparer `R` à la cible |

Le programme stocké est `02 2A 01 05 03 03 FF`, soit pour chaque caractère : `R = rol8((c ^ 0x2A) + 5, 3)`. La cible (9 octets) est `EA A2 0B A2 EA F2 A2 19 E8`.

On inverse l'opération, du dernier vers le premier opérateur ([scripts/solve_machine.py](scripts/solve_machine.py)) :

```python
def ror8(v, k): return ((v >> k) | (v << (8 - k))) & 0xFF
cible = [0xEA,0xA2,0x0B,0xA2,0xEA,0xF2,0xA2,0x19,0xE8]
mdp = "".join(chr(((ror8(R, 3) - 5) & 0xFF) ^ 0x2A) for R in cible)
# -> reverse42
```

```
$ ./machine reverse42
FLAG{vm_reverse42}
```

Inverser une VM revient toujours à inverser chaque opération : `rol` devient `ror`, `add` devient `sub`, et `xor` est son propre inverse.

### Défi 3 : oignon

Au lancement, le programme appelle `ptrace(PTRACE_TRACEME, ...)` : si un débogueur est déjà attaché, l'appel échoue et le programme affiche `Nope.`. Trois façons de neutraliser cela (chapitre 5 de la partie 3) : forcer la valeur de retour dans GDB (`catch syscall ptrace`, puis `set $rax = 0`), précharger un faux `ptrace` avec `LD_PRELOAD`, ou patcher l'appel dans le binaire.

Mais la voie **statique** est la plus propre et évite l'anti-debug entièrement. Dans `objdump -s -j .data oignon`, on relève deux tableaux de 11 octets, et dans le désassemblage, juste avant le `memcmp`, une boucle qui les combine par `xor`. Le mot de passe attendu est donc `couche_a XOR couche_b` ([scripts/solve_oignon.py](scripts/solve_oignon.py)) :

```python
a = [0x3B,0x1E,0x0A,0x11,0x0B,0x1C,0x56,0x4C,0x0C,0x12,0x16]
b = [0x4B,0x7B,0x66,0x64,0x79,0x79,0x09,0x7D,0x3F,0x21,0x21]
"".join(chr(x ^ y) for x, y in zip(a, b))   # -> pelure_1337
```

```
$ ./oignon pelure_1337
FLAG{oignon_pelure_1337}
```

La leçon : un secret « jamais stocké en clair » mais recomposé à l'exécution par une opération réversible se retrouve toujours statiquement, sans jamais lancer le binaire ni affronter sa protection.

## Cas de malware {#cas-malware}

Analyse complète de `specimen`. Le rapport final est sur la page [rapport d'analyse](../partie4-capstone/22-rapport-type.md) ; voici la démarche qui y mène.

### 1. Triage statique

`file specimen` confirme un ELF 64 bits *stripped*. `strings specimen` montre quelques chaînes en clair (`/tmp/.syscache.lock`, `/tmp/.syscache.log`, le format `beacon host=%s c2=%s campaign=%s`) mais **ni domaine, ni flag, ni identifiant de campagne** : ces éléments sont donc chiffrés dans le binaire. La présence du format `beacon ...` oriente déjà vers un comportement de balise.

### 2. Cartographie dynamique (bac à sable)

`strace -f ./specimen` donne la carte des étapes :

```
ptrace(PTRACE_TRACEME, ...)            = 0
openat(AT_FDCWD, "/etc/hostname", ...) = 3      # reconnaissance
openat(AT_FDCWD, "/tmp/.syscache.lock", O_WRONLY|O_CREAT|O_EXCL, 0644)
openat(AT_FDCWD, "/tmp/.syscache.log", ...)     # balise
socket(AF_INET, SOCK_STREAM, ...)      = 4
connect(4, {127.0.0.1:4444}, ...)      = -1 ECONNREFUSED   # C2 (echoue)
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, ...)
mprotect(..., PROT_READ|PROT_EXEC)     # la page devient executable
write(1, "charge active: FLAG{...}\n", 39)
```

Chaque ligne se retrouve ensuite dans le désassemblage. L'anti-debug `ptrace` n'empêche pas `strace` (qui n'est pas un *debugger* au sens de `ptrace` concurrent ici), mais il faut le neutraliser pour attacher GDB.

### 3. Déchiffrement des chaînes (statique)

Les chaînes sensibles sont stockées chiffrées et déchiffrées en mémoire par un `xor` avec `0x3C` (la clé se lit dans le désassemblage de la routine `dexor`). On relève les tableaux dans `.data`/`.rodata` avec `objdump -s`, puis on rejoue le XOR à froid ([scripts/decode_specimen.py](scripts/decode_specimen.py)) :

```python
dom  = bytes(b ^ 0x3C for b in dom_enc).rstrip(b"\x00").decode()
camp = bytes(b ^ 0x3C for b in camp_enc).rstrip(b"\x00").decode()
# dom  -> update.sync.example.invalid
# camp -> CAMP-2026-ORANGE
```

### 4. Extraction de la charge (statique)

La séquence `mmap` (lecture-écriture) → boucle `xor 0x6B` → `mprotect` (lecture-exécution) → `call` sur la zone est la signature du loader. On relève le tableau `charge` (65 octets) et la clé `0x6B` dans le désassemblage, on rejoue le XOR, et on désassemble le résultat sans jamais l'exécuter :

```bash
objdump -D -b binary -m i386:x86-64 -M intel charge_dechiffree.bin
```

```
jmp    0x15                 ; jmp-call-pop : va chercher l'adresse de la chaine
pop    rsi                  ; rsi = adresse du message
mov    eax, 0x1             ; write
mov    edi, 0x1             ; fd = 1
mov    edx, 0x27            ; longueur 39
syscall
ret                         ; rend la main au loader
```

La charge est donc un simple `write` d'un message suivi d'un `ret` : inoffensive. Le message, lisible dans les octets déchiffrés, est `charge active: FLAG{malware_decode_ok}`.

### 5. Synthèse

Toutes les réponses de la mission sont réunies : reconnaissance (`/etc/hostname`), marqueur (`/tmp/.syscache.lock`), balise (`/tmp/.syscache.log` avec le C2 `update.sync.example.invalid` et la campagne `CAMP-2026-ORANGE`), réseau (`127.0.0.1:4444`), anti-debug (`ptrace`), et charge chiffrée (XOR `0x6B`) extraite **sans exécution**. Le tableau d'IOC et le rapport final en découlent directement (voir la page [rapport d'analyse](../partie4-capstone/22-rapport-type.md)).

!!! success "Le réflexe à retenir"
    L'échantillon n'a jamais eu besoin d'être réellement lancé pour être entièrement compris. C'est tout l'objet du cours : lire le code machine, reconnaître les schémas, et déchiffrer à froid. De là, les CTF de reverse et l'analyse de malware ne sont plus qu'une question d'entraînement.
