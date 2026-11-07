# Anti-reverse avancé

!!! info "Chapitre d'approfondissement"
    Approfondissement de la partie 2. Le chapitre 12 a introduit les protections de base (`ptrace`, timing, détection de point d'arrêt). Ici, on combine plusieurs techniques, on en ajoute de nouvelles, et surtout on apprend à **toutes les contourner**. Objectif strictement défensif : analyser et neutraliser des protections pour le reverse légitime, pas aider à protéger un logiciel malveillant. Tous les exemples sont compilés et testés ; les sources sont dans `anti-reverse/`.

## 18.1 Empiler les détections

Un vrai programme protégé ne se contente pas d'un seul test : il en combine plusieurs, pour qu'en neutraliser un ne suffise pas. Le binaire `protege` en empile quatre, et n'affiche son flag que si aucun ne se déclenche :

1. **TracerPid** (`/proc/self/status`). Le noyau y publie le PID du traceur attaché, ou 0 si aucun. Un simple `fopen`/lecture révèle la présence d'un débogueur (y compris GDB ou `strace`).
2. **LD_PRELOAD**. Beaucoup d'outils de contournement passent par `LD_PRELOAD` ; tester `getenv("LD_PRELOAD")` les repère.
3. **Timing avec `rdtsc`**. L'instruction `rdtsc` lit le compteur de cycles. Si un bloc censé être très rapide prend un temps énorme, c'est qu'on exécute pas à pas sous un débogueur.
4. **`ptrace(PTRACE_TRACEME)`**. Un processus ne peut être tracé qu'une fois : si un débogueur est déjà attaché, l'appel échoue.

!!! warning "Ordre des tests : un piège à connaître"
    `ptrace(PTRACE_TRACEME)` fait du processus parent un traceur. Si on l'appelle **avant** le test TracerPid, ce dernier verra alors un traceur (le parent) et se déclenchera à tort. Dans `protege`, `ptrace` est donc testé **en dernier**. C'est un détail que l'on retrouve en reverse : l'ordre des protections n'est pas anodin.

Exécuté normalement, `protege` affiche `FLAG{anti_debug_contourne}`. Sous `strace` ou GDB, il affiche « debogueur detecte (TracerPid) ». Avec `LD_PRELOAD` défini, « LD_PRELOAD detecte ».

## 18.2 Contourner, méthode 1 : LD_PRELOAD

On peut remplacer une fonction de la libc par la nôtre, chargée en priorité via `LD_PRELOAD`. Ici, une petite bibliothèque `faux.so` fait deux choses :

```c
__attribute__((constructor))
static void init(void){ unsetenv("LD_PRELOAD"); }   // défait le test LD_PRELOAD
long ptrace(int req, ...){ return 0; }               // défait le test ptrace
```

Le constructeur s'exécute **avant** `main` et retire `LD_PRELOAD` de l'environnement, si bien que le test `getenv("LD_PRELOAD")` ne voit rien. Et notre `ptrace` masque celui de la libc, renvoyant toujours 0.

```
$ LD_PRELOAD=./faux.so ./protege
FLAG{anti_debug_contourne}
```

Deux protections tombent d'un coup. C'est la force de `LD_PRELOAD` : neutraliser des fonctions sans toucher au binaire.

## 18.3 Contourner, méthode 2 : le patch

On peut aussi modifier le binaire pour neutraliser un test. Le test TracerPid se termine par un saut conditionnel vers le bloc « détecté » :

```asm
test eax, eax
jne  main+0xa1      ; 0f 85 8b 00 00 00  -> saute si TracerPid != 0
```

En remplaçant les six octets du `jne` par des `NOP` (`0x90`), le saut disparaît et le test est toujours « passé ». Le script `patch_tracerpid.py` le fait automatiquement.

!!! tip "Les couches se cumulent"
    Patcher le seul test TracerPid ne suffit pas sous `strace` : le programme atteint alors le test `ptrace`, qui se déclenche à son tour. Pour une analyse dynamique complète, il faut **combiner** les contournements (par exemple patch TracerPid + `faux.so` pour `ptrace`). C'est la règle du jeu face à un binaire bien protégé : chaque couche demande sa parade.

## 18.4 Contourner, méthode 3 : le débogueur lui-même

Depuis GDB, on neutralise un test sans modifier le fichier :

- intercepter l'appel système : `catch syscall ptrace`, puis au retour `set $rax = 0` ;
- forcer le résultat d'un test : poser un point d'arrêt juste après le `call`, puis `set $eax = 0` avant le `test` ;
- sauter par-dessus un bloc : `set $pc = adresse_apres_le_test`.

Ces manipulations, vues au chapitre 12, s'appliquent à chaque couche repérée dans le désassemblage.

## 18.5 L'anti-désassemblage

Au-delà de l'anti-debug, un binaire peut gêner la **lecture statique** elle-même. La technique la plus simple est l'**octet de leurre** : placer, juste après un saut inconditionnel, un octet qui n'est jamais exécuté mais qui amorce une instruction longue, désalignant un désassembleur à balayage linéaire.

```asm
_start:
    jmp court
    db 0xE8          ; jamais exécuté ; 0xE8 = début d'un 'call rel32' sur 5 octets
court:
    mov eax, 60
    ...
```

Le `jmp` saute par-dessus l'octet `0xE8`. Mais un désassembleur linéaire, qui décode octet après octet, prend `0xE8` pour un `call` et « avale » les quatre octets suivants (de vraies instructions), produisant du charabia à partir de là. Les variantes incluent les sauts **au milieu** d'une instruction (le même octet est lu différemment selon le point d'entrée) et les *opaque predicates* (une condition toujours vraie ou toujours fausse, mais que l'analyse ne sait pas trancher, qui noie le vrai chemin parmi de faux).

La parade est le **désassemblage par flot de contrôle** (suivre réellement les sauts), ce que font Ghidra et les désassembleurs récursifs, au lieu du balayage linéaire de `objdump -d`. En cas de doute, vérifier dynamiquement quelles instructions s'exécutent vraiment (`stepi` dans GDB) lève l'ambiguïté.

## Exercices gradués

Sources dans `anti-reverse/` (`protege.c`, `faux.c`, `antidis.asm`, `patch_tracerpid.py`).

**Exercice 18.1 (niveau 1)** Compiler `protege` (`gcc -O1 -no-pie protege.c -o protege`), le lancer normalement (flag), puis sous `strace` et sous GDB. Quel test se déclenche dans chaque cas, et pourquoi ?

**Exercice 18.2 (niveau 2)** Écrire et compiler `faux.so`, puis contourner `protege` avec `LD_PRELOAD=./faux.so`. Expliquer précisément quelles deux protections sont neutralisées, et par quel mécanisme chacune.

**Exercice 18.3 (niveau 2)** Repérer dans le désassemblage le saut conditionnel du test TracerPid, le neutraliser (patch en `NOP`), et vérifier. Pourquoi le binaire patché est-il quand même détecté sous `strace` ?

**Exercice 18.4 (niveau 3)** Combiner deux contournements pour faire passer `protege` sous un traceur (par exemple patch TracerPid + `faux.so` pour `ptrace`). Vérifier que le flag s'affiche malgré la présence du traceur.

**Exercice 18.5 (niveau 2)** Écrire un faux `getppid` (ou `ptrace`) en `LD_PRELOAD` et vérifier avec un petit programme témoin que c'est bien votre version qui est appelée. En quoi est-ce plus souple qu'un patch du binaire ?

**Exercice 18.6 (niveau 3)** Assembler `antidis.asm`, l'exécuter (code de retour attendu 7), puis comparer `objdump -d` (balayage linéaire) et une lecture pas à pas dans GDB. Montrer que l'octet `0xE8` n'est jamais exécuté et explique la désynchronisation.

Solutions : [solutions/partie2.md](../solutions/partie2.md#anti-reverse)

## Suivi

- [ ] Chapitre anti-reverse : lecture
- [ ] Chapitre anti-reverse : contournements (LD_PRELOAD, patch, GDB)
- [ ] Chapitre anti-reverse : anti-désassemblage compris
