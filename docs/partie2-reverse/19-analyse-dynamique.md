# Analyse dynamique avancée

!!! info "Chapitre d'approfondissement"
    Approfondissement de la partie 3. Observer un programme pendant qu'il s'exécute révèle souvent en quelques secondes ce qu'une analyse statique mettrait une heure à établir. Ce chapitre approfondit `strace` et `ltrace`, puis introduit `Frida`, qui ne se contente pas d'observer : il **modifie** l'exécution.

!!! note "Note sur l'environnement"
    Les exemples `strace` ci-dessous sont réels et testés. Pour `ltrace`, on montre aussi le même résultat avec GDB, qui fonctionne dans tous les cas. `strace`, `ltrace` et `Frida` sont tous fournis dans l'image du laboratoire (`reverse-lab`, voir l'installation sur Mac) : vous pouvez y rejouer directement les exemples, y compris le script Frida (`frida -l hook_strcmp.js ./motdepasse essai`), sans rien installer de plus.

## 11.1 strace en profondeur

`strace` journalise tous les **appels système** d'un programme : ouvertures de fichiers, lectures, connexions réseau, mémoire. On lit la trace comme une histoire. Options essentielles :

- `-e trace=openat,connect` : ne garder que certains appels (ici fichiers et réseau) ;
- `-f` : suivre les processus fils et les threads (`fork`, `clone`) ;
- `-k` : afficher la pile d'appels à chaque syscall (d'où vient l'appel) ;
- `-p PID` : s'attacher à un processus déjà lancé.

Sur un programme qui lit un fichier puis tente une connexion :

```
$ strace -e trace=openat,connect ./activite
openat(AT_FDCWD, "/etc/hostname", O_RDONLY) = 3
connect(3, {sa_family=AF_INET, sin_port=htons(8080),
            sin_addr=inet_addr("127.0.0.1")}, 16) = -1 ECONNREFUSED
```

En deux lignes, on sait que le programme lit le nom de la machine puis essaie de joindre `127.0.0.1:8080`. C'est exactement la méthode pour cartographier un échantillon inconnu avant même de le désassembler.

!!! tip "L'anti-debug change la trace"
    Le `specimen` du capstone appelle `ptrace(PTRACE_TRACEME)` : lancé sous `strace`, l'appel échoue (le processus est déjà tracé), et le programme s'arrête avant d'agir. La trace s'interrompt alors tôt. C'est le lien direct avec le chapitre précédent : une protection anti-debug fausse l'analyse dynamique, et il faut la neutraliser (par exemple avec un faux `ptrace`) pour que `strace` montre le vrai comportement.

## 11.2 ltrace : les appels de bibliothèque

Là où `strace` montre les appels système (frontière avec le noyau), `ltrace` montre les appels de **fonctions de bibliothèque** : `strcmp`, `malloc`, `memcpy`, fonctions de chiffrement... C'est redoutable sur les crackmes, car il révèle souvent la comparaison décisive.

Pour un programme qui compare le mot de passe avec `strcmp`, `ltrace ./motdepasse essai` afficherait :

```
strcmp("essai", "sX3-reverse!")   = -1
```

Le second argument est le mot de passe attendu, **en clair**. Sans `ltrace`, on obtient le même résultat avec GDB en s'arrêtant sur l'appel à `strcmp` et en lisant ses deux arguments (`rdi` et `rsi`) :

```
(gdb) break *<adresse du call strcmp dans main>
(gdb) run essai
(gdb) x/s $rdi      ->  "essai"
(gdb) x/s $rsi      ->  "sX3-reverse!"
```

Les deux chaînes comparées apparaissent, dont le mot de passe. Limites de `ltrace` : il ne voit que les fonctions **liées dynamiquement** (un binaire statique ou une fonction *inlinée* ne montre rien), et il peut être bruyant. GDB, lui, fonctionne dans tous les cas, au prix d'un point d'arrêt à placer.

## 11.3 Frida : observer ET modifier

`strace` et `ltrace` observent. **Frida** instrumente : il injecte du code dans le processus en cours et permet d'intercepter n'importe quelle fonction, de lire et **modifier** ses arguments et sa valeur de retour, en direct, sans recompiler ni patcher le binaire. C'est l'outil de choix pour le reverse dynamique moderne (applications, jeux, mobile).

Le cœur est `Interceptor.attach`, qui pose un hook sur une fonction :

```javascript
Interceptor.attach(Module.getExportByName(null, 'strcmp'), {
    onEnter(args) {
        console.log('strcmp("' + args[0].readUtf8String() + '", "'
                              + args[1].readUtf8String() + '")');
    },
    onLeave(retval) {
        retval.replace(0);   // force l'egalite : le test passe toujours
    }
});
```

`onEnter` lit les arguments (ce que ferait `ltrace`), `onLeave` réécrit la valeur de retour. Ici, forcer `strcmp` à renvoyer 0 fait croire au programme que le mot de passe est toujours bon : le test est contourné sans toucher au binaire. Lancement (dans le laboratoire) :

```
$ frida -l hook_strcmp.js ./motdepasse essai
```

C'est le passage clé : **ce que `ltrace` observe, Frida permet de le changer**. Observer révèle le mot de passe ; modifier la valeur de retour rend la vérification caduque.

## 11.4 Quel outil pour quoi

| Besoin | Outil |
|---|---|
| Voir fichiers, réseau, mémoire (syscalls) | `strace` |
| Voir les appels de bibliothèque (strcmp, malloc...) | `ltrace` (ou GDB) |
| Lire un argument précis à un instant précis | GDB (point d'arrêt) |
| Modifier arguments / retour en direct, hooker | Frida |

La démarche type : `strace`/`ltrace` pour comprendre vite ce que fait le programme, GDB pour inspecter finement un point, Frida pour agir sur l'exécution.

## Exercices gradués

Sources dans `dynamique/` (`motdepasse.c`, `activite.c`, `hook_strcmp.js`). `strace`, `ltrace` et `Frida` sont tous disponibles dans l'image du laboratoire (`reverse-lab`) ; GDB peut aussi remplacer `ltrace` au besoin.

**Exercice 11.1 (niveau 1)** Compiler `activite.c` et lancer `strace -e trace=openat,connect ./activite`. Décrire en une phrase ce que fait le programme, d'après la seule trace.

**Exercice 11.2 (niveau 2)** Compiler `motdepasse.c`. Retrouver le mot de passe attendu **sans lire la source** : avec `ltrace ./motdepasse essai` si disponible, sinon avec GDB en lisant les deux arguments de `strcmp`. Vérifier que le mot de passe trouvé donne le flag.

**Exercice 11.3 (niveau 2)** Lancer le `specimen` du capstone sous `strace`. Pourquoi la trace s'arrête-t-elle tôt ? Quel appel système en est la cause, et comment le neutraliser pour voir le vrai comportement ?

**Exercice 11.4 (niveau 2)** Sur un programme qui lit un fichier, utiliser `strace -f` pour confirmer qu'aucun processus fils n'est créé, puis `strace -e trace=openat` pour lister tous les fichiers ouverts, y compris les bibliothèques chargées au démarrage.

**Exercice 11.5 (niveau 3)** Avec Frida et `hook_strcmp.js`, journaliser l'appel à `strcmp` de `motdepasse` (observer le mot de passe), puis activer le `retval.replace(0)` et vérifier que n'importe quelle entrée est désormais acceptée. Expliquer la différence entre observer et modifier.

**Exercice 11.6 (niveau 3)** Comparer ce que montrent `strace` et `ltrace` (ou GDB) sur `motdepasse` : lequel révèle le mot de passe, et pourquoi l'autre ne le voit pas ?

Solutions : [solutions/partie2.md](../solutions/partie2.md#analyse-dynamique)

## Suivi

- [ ] Partie 3, chapitre analyse dynamique : lecture
- [ ] Partie 3, chapitre analyse dynamique : strace et ltrace/GDB pratiqués
- [ ] Partie 3, chapitre analyse dynamique : Frida essayé (dans le laboratoire)
