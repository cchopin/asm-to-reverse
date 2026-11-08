# Écrire des règles YARA

!!! info "Chapitre d'approfondissement"
    Dernier chapitre d'approfondissement de la partie 3. Jusqu'ici, chaque analyse restait dans votre tête ou dans un rapport. **YARA** permet de figer ce que vous avez compris en une **règle réutilisable** : une signature qui reconnaît un échantillon, une famille de malware, ou une caractéristique (packing, IOC, format) sur des milliers de fichiers d'un coup. C'est l'outil qui transforme une analyse ponctuelle en détection durable.

!!! note "Note sur l'environnement"
    `yara-python` (version 4.5) **est** installé dans le conteneur du cours : toutes les règles ci-dessous sont **réelles et testées** sur les binaires du cours (`demo` et `packed` du chapitre 13, et `coffre_v5`, le faux malware du chapitre 6). Vous pouvez les rejouer avec `python3` et le module `yara`. En ligne de commande sur votre machine, c'est l'outil `yara` (`yara regle.yar fichier`) ou `yara -r regle.yar dossier/` pour balayer un dossier entier. Les sources sont dans `yara/`.

## 14.1 À quoi sert YARA

YARA décrit des **motifs** (des chaînes, des octets, des conditions) et dit « ce fichier correspond » ou non. On s'en sert pour :

- **classer** un échantillon inconnu (« est-ce de la famille X ? ») ;
- **détecter** une caractéristique (packé, chiffré, contient tel IOC) ;
- **trier** (triage) un gros volume de fichiers et isoler ceux qui méritent une analyse manuelle.

Une règle est un petit document structuré. Son squelette :

```yara
rule nom_de_la_regle
{
    meta:
        description = "ce que fait la regle"
        auteur      = "vous"
    strings:
        $a = "une chaine"
        $b = { 48 8B ?? }
    condition:
        $a and $b
}
```

Trois sections : `meta` (informations libres, non testées), `strings` (les motifs à chercher, préfixés par `$`), `condition` (l'expression booléenne qui décide du verdict). Seule `condition` est obligatoire.

## 14.2 Les trois types de motifs

**Chaîne de texte.** La plus simple. On peut la moduler :

```yara
$t = "reverse42"              // exacte
$i = "Reverse42" nocase       // insensible a la casse
$w = "reverse42" wide         // encodee UTF-16 (frequent sous Windows)
$f = "reverse42" fullword     // mot entier, pas une sous-chaine
```

**Octets (hex).** Pour une suite d'octets précise, avec des **jokers** là où les octets varient (adresses, registres) :

```yara
$h = { 48 8B 05 ?? ?? ?? ?? }   // mov rax, [rip+????] : le deplacement varie
$j = { 55 48 89 E5 [4-16] C3 }  // push rbp; mov rbp,rsp ... ret, 4 a 16 octets entre
```

`??` est un octet quelconque ; `[4-16]` est un intervalle de longueur variable ; `( 74 | 75 )` accepte l'un ou l'autre octet. C'est idéal pour signer une **portion de code** stable malgré les adresses qui changent.

**Expression régulière.** Pour une forme plus souple :

```yara
$r = /https?:\/\/[a-z0-9.-]+\.(invalid|example)/
```

## 14.3 La condition

C'est le cœur. Quelques formes utiles, toutes testées :

```yara
condition:
    $a                       // le motif $a est present
    any of them              // au moins un motif
    all of them              // tous les motifs
    2 of ($a, $b, $c)        // au moins 2 des trois
    $a and not $b            // $a oui, $b non
    $a at 0                  // $a exactement au decalage 0
    $a in (0..1024)          // $a dans les 1024 premiers octets
    filesize < 100KB         // taille du fichier
```

On combine avec `and`, `or`, `not` et des parenthèses. Et surtout, on peut lire des octets à une position donnée, ce qui sert à reconnaître un **format par son magic** :

```yara
uint32(0) == 0x464c457f    // ELF  : 7f 45 4c 46 en petit-boutiste
uint16(0) == 0x5a4d        // PE   : 'MZ' (4d 5a) en petit-boutiste
```

!!! tip "Petit-boutiste, donc les octets semblent inversés"
    `uint32(0)` lit 4 octets en little-endian. Le fichier ELF commence par `7f 45 4c 46`, mais lu en entier sur 32 bits cela donne `0x464c457f` (octets inversés). De même `MZ` = `4d 5a` se lit `0x5a4d` en `uint16`. C'est une source d'erreur classique quand on débute.

## 14.4 Exemple 1 : signer un binaire précis

On veut une règle qui reconnaît `demo` (il contient la chaîne `reverse42`) sans se déclencher sur sa version packée (où le code est compressé, la chaîne n'apparaît plus en clair) :

```yara
rule demo_reverse42 {
    strings: $s = "reverse42"
    condition: $s
}
```

Testée réellement :

```
demo   -> match
packed -> pas de match   (la chaine est dans un blob compresse)
```

C'est le lien direct avec le chapitre 13 : une chaîne visible en `.rodata` donne une signature triviale ; dès que le binaire est packé, cette signature tombe, et il faut signer autrement (entropie, voir plus bas).

## 14.5 Exemple 2 : reconnaître un format

```yara
rule est_un_elf {
    meta: description = "fichier ELF, quel que soit son contenu"
    condition: uint32(0) == 0x464c457f
}
```

Testée : `demo`, `packed`, `coffre_v5` correspondent tous ; un fichier texte, non. Même principe pour un PE avec `uint16(0) == 0x5a4d`. On trie ainsi un dossier par format avant toute autre analyse.

## 14.6 Exemple 3 : détecter le packing par l'entropie

Le module `math` calcule l'entropie de Shannon (vue au chapitre 9). Du code et des chaînes normales ont une entropie modérée ; des données **compressées ou chiffrées** s'approchent de 8 bits/octet. Une règle qui signe « probablement packé » :

```yara
import "math"
rule packe_probable {
    meta: description = "entropie elevee = donnees compressees ou chiffrees"
    condition: math.entropy(0, filesize) >= 7.2
}
```

Testée sur des données réelles :

```
donnees normales (texte)   entropie 2.65  -> pas de match
donnees compressees (zlib) entropie 7.91  -> match
donnees aleatoires         entropie 7.95  -> match
```

Le seuil 7.2 attrape le compressé et le chiffré sans lever de faux positif sur du texte. `math.entropy(offset, taille)` peut cibler une région précise (par exemple une seule section), ce qui est plus fin qu'une mesure sur tout le fichier, surtout sur un petit binaire où le remplissage à zéro fait baisser la moyenne.

## 14.7 Exemple 4 : attraper une famille par ses IOC (cas malware)

C'est l'usage phare de YARA. Le `coffre_v5` du chapitre 6 cache sa configuration (le C2 `http://c2.coffre.invalid:8443/gate.php`, la campagne `CAMPAGNE-ROUGE-07`) dans des tableaux globaux **chiffrés par XOR 0x37**. En clair, `strings` ne les voit pas. Mais YARA sait chercher une chaîne **à travers un XOR** grâce au modificateur `xor` :

```yara
rule coffre_v5_campagne_rouge {
    meta:
        description = "famille 'coffre' : C2 et campagne, chaines XORees"
        reference   = "partie 3, chapitre 6 (fil rouge coffre v5)"
    strings:
        $c2   = "c2.coffre.invalid" xor(0x37)
        $camp = "CAMPAGNE-ROUGE-07" xor(0x37)
    condition:
        any of them
}
```

Testée : `coffre_v5` (même strippé) correspond, et YARA indique quels motifs (`$c2`, `$camp`) ont été trouvés. C'est la bascule décisive : ce que l'on a **déchiffré à la main** au chapitre 6 devient une règle qui retrouvera **toute** la famille, y compris des variantes qu'on n'a jamais vues, tant qu'elles réutilisent ces IOC et ce XOR. Avec `xor(0x00-0xff)`, YARA essaie même toutes les clés d'un octet.

!!! danger "Rester défensif"
    On écrit ces règles pour **détecter et classer**, jamais pour aider à échapper à la détection. Une bonne règle est spécifique (peu de faux positifs) et documentée (`meta`), pour qu'une autre personne puisse la relire et la faire confiance.

## 14.8 Les modules pe et elf

Au-delà des octets bruts, YARA offre des **modules** qui comprennent la structure d'un fichier. Le module `pe` (conceptuel ici, non testable faute de PE dans le conteneur) donne accès à la table d'import, en lien direct avec le chapitre 13 :

```yara
import "pe"
rule pe_telechargeur {
    condition:
        pe.imports("wininet.dll", "InternetReadFile") and
        pe.imports("kernel32.dll", "CreateProcessA")
}
```

Cette règle signe un comportement (télécharge puis exécute) plutôt qu'une chaîne précise : elle résiste au changement de C2 ou de nom de fichier. Le module `elf` expose de même le type, le point d'entrée, les sections d'un ELF. Signer un **comportement** (imports, structure) est plus robuste que signer une chaîne, qui change d'une variante à l'autre.

## 14.9 De l'analyse à la règle

La démarche de tout ce cours se referme ici. Chaque technique de reverse produit un élément signable :

| Ce qu'on a trouvé | Ce qu'on signe dans YARA |
|---|---|
| Une chaîne en clair (chapitres 1 et 13) | `strings: $s = "..."` |
| Un bout de code stable (partie 2, chapitre 7 ; chapitre 2) | motif hex avec jokers `{ 48 8B ?? }` |
| Un IOC chiffré (chapitres 6 et 8) | chaîne avec `xor(...)` |
| Un binaire packé (chapitre 9) | `math.entropy(...) >= 7.2` |
| Des capacités (imports, chapitre 13) | module `pe` / `elf` |

On passe ainsi d'une analyse qui ne vaut que pour un fichier à une règle qui protège contre toute une famille. C'est la compétence qui fait le pont entre le reverse et la détection en production.

## Exercices gradués

Sources dans `yara/`. Tout est testable ici via `python3` + module `yara` (ou `yara` en ligne de commande sur votre machine). Les binaires de test se construisent comme au chapitre 13 (`demo`, `packed`) et au chapitre 6 (`coffre_v5`).

**Exercice 14.1 (niveau 1)** Écrire et tester une règle qui correspond à `demo` (via la chaîne `reverse42`) mais pas à `packed`. Expliquer pourquoi `packed` échappe à la règle, et ce que cela dit des limites d'une signature par chaîne.

**Exercice 14.2 (niveau 1)** Écrire une règle qui reconnaît **tout** fichier ELF par son magic (`uint32(0) == 0x464c457f`). La tester sur `demo` (match) et sur un fichier texte (pas de match). Écrire la condition équivalente pour un PE.

**Exercice 14.3 (niveau 2)** Avec le module `math`, écrire une règle « packé probable » (`math.entropy(0, filesize) >= 7.2`). La tester sur trois fichiers que vous fabriquez : du texte répétitif, des données compressées (`zlib`), des données aléatoires. Donner l'entropie de chacun et dire lesquels déclenchent la règle.

**Exercice 14.4 (niveau 2)** Écrire une règle à **plusieurs motifs** avec une condition `2 of them` (par exemple deux chaînes présentes dans un même binaire). Vérifier qu'elle ne se déclenche que si au moins deux motifs sont présents. Comparer avec `all of them` et `any of them`.

**Exercice 14.5 (niveau 3)** Écrire une règle qui attrape les IOC de `coffre_v5` (le C2 et la campagne, retrouvés au chapitre 6), sachant qu'ils sont chiffrés par XOR 0x37. Utiliser le modificateur `xor(0x37)` sur les chaînes en clair. Vérifier le match, et lister les motifs trouvés. Bonus : remplacer par `xor(0x00-0xff)` et expliquer ce que YARA fait alors.

**Exercice 14.6 (niveau 2)** Écrire un motif **hex avec jokers** qui signe le prologue de fonction `push rbp; mov rbp, rsp` (`{ 55 48 89 E5 }`) et le tester sur un binaire non optimisé du cours. Expliquer pourquoi un joker (`??`) serait nécessaire pour signer une instruction contenant une adresse.

**Exercice 14.7 (niveau 3, conceptuel PE)** Écrire une règle utilisant le module `pe` qui signe un téléchargeur : imports de `wininet.dll!InternetReadFile` et de `kernel32.dll!CreateProcessA`. Expliquer pourquoi signer des imports est plus robuste que signer une chaîne, et faire le lien avec la table d'import vue au chapitre 13.

**Exercice 14.8 (niveau 3)** Prendre un crackme du CTF maison que vous avez résolu, et en écrire une règle YARA qui le reconnaît de façon spécifique (combinaison de chaîne, de taille et éventuellement d'un bout de code). Vérifier qu'elle matche ce crackme et pas les autres. C'est l'exercice de synthèse : transformer votre analyse en détection.

Solutions : [solutions/partie2.md](../solutions/partie2.md#yara)

## Suivi

- [ ] Partie 3, chapitre YARA : lecture
- [ ] Partie 3, chapitre YARA : règles écrites et testées (format, entropie, IOC)
- [ ] Partie 3, chapitre YARA : passage analyse -> règle réutilisable compris
