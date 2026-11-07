# C++ et fonctions virtuelles

!!! info "Chapitre d'approfondissement"
    Beaucoup de binaires réels (jeux, logiciels propriétaires, certains malwares) sont écrits en C++. Le code compilé a quelques particularités qui déroutent si on ne les connaît pas : des noms de symboles illisibles, un argument caché `this`, et surtout les **tables de fonctions virtuelles** (*vtables*). Ce chapitre apprend à les reconnaître. Tous les listings sont réels (`g++` puis `objdump`).

## 12.1 Les noms décorés (name mangling)

Le C++ autorise la surcharge (plusieurs fonctions de même nom, signatures différentes). Pour les distinguer dans le binaire, le compilateur **décore** les noms en y encodant la classe et les types. Ainsi `calcule(const Forme*)` devient le symbole `_Z7calculePK5Forme`, et `Carre::aire() const` devient `_ZNK5Carre4aireEv`.

L'outil `c++filt` fait la traduction inverse :

```bash
$ echo _ZNK5Carre4aireEv | c++filt
Carre::aire() const
$ echo _Z7calculePK5Forme | c++filt
calcule(Forme const*)
```

Dès qu'on voit des symboles commençant par `_Z`, c'est du C++. Passer tout un désassemblage dans `c++filt` (ou activer le démanglage dans Ghidra) rend les noms lisibles.

## 12.2 Le pointeur this

Une méthode non statique reçoit un premier argument implicite : `this`, le pointeur sur l'objet. Il est passé dans `rdi`, et les vrais arguments décalent d'un cran (`rsi`, `rdx`...). Une méthode `int Carre::aire() const` se lit donc comme une fonction C `int aire(const Carre *this)`.

```asm
; Carre::aire() const   (ici en -O2)
mov  eax, [rdi+0xc]   ; this->cote
imul eax, eax         ; cote * cote
ret
```

`rdi` est `this`, et `[rdi+0xc]` est le champ `cote`. Voir une fonction qui travaille immédiatement à partir de `[rdi+...]` sans que `rdi` soit un argument déclaré est la signature d'une méthode C++.

## 12.3 La disposition d'un objet avec vtable

Dès qu'une classe a au moins une fonction virtuelle, chaque objet commence par un **pointeur de vtable** (le *vptr*), caché à l'offset 0. Les champs déclarés viennent après. Pour notre `Carre` (qui hérite de `Forme { int tag; }` et ajoute `int cote;`) :

| Offset | Contenu |
|---|---|
| 0 | vptr (pointeur vers la vtable de Carre) |
| 8 | `tag` (hérité de Forme) |
| 12 (`0xc`) | `cote` |

C'est pourquoi `cote` est à l'offset `0xc` et non `0` : les 8 premiers octets sont le vptr, puis `tag`. Un objet dont le tout premier champ est un pointeur vers `.rodata`/`.data.rel.ro` est presque toujours un objet polymorphe C++.

## 12.4 La vtable

La vtable est un tableau de pointeurs de fonctions, une entrée par méthode virtuelle, stocké en lecture seule. Ici :

```bash
$ objdump -t forme | grep _ZTV5Carre
... _ZTV5Carre            # "vtable for Carre"
```

La vtable de `Carre` contient les adresses de ses méthodes virtuelles effectives : son `aire()` (celui de `Carre`, pas celui de `Forme`) et son destructeur. Chaque objet `Carre` pointe, par son vptr, vers cette table. Deux objets de classes différentes ont des vptr différents : c'est ce qui permet le comportement polymorphe.

## 12.5 L'appel virtuel

C'est le motif central. `int calcule(const Forme *f) { return f->aire(); }` ne peut pas savoir à la compilation quelle `aire()` appeler (celle de `Forme` ? de `Carre` ? d'une autre sous-classe ?). Il passe donc par la vtable, en `-O0` :

```asm
mov rax, [rbp-0x8]   ; rax = f (this)
mov rax, [rax]       ; rax = vptr = f->vtable
mov rdx, [rax]       ; rdx = vtable[0] = adresse de aire()
mov rdi, rax_de_f    ; 1er argument = this
call rdx             ; appel indirect
```

La séquence **« charger `[this]` pour avoir le vptr, puis appeler via `[vptr + offset]` »** est la signature infaillible d'un appel virtuel. L'`offset` dans la vtable indique quelle méthode virtuelle (0 pour la première, 8 pour la deuxième, etc.).

À comparer avec un **appel direct** (méthode non virtuelle ou fonction libre), qui est un simple `call adresse_fixe` : là, la cible est connue dès la compilation. Distinguer les deux dit immédiatement si la méthode est virtuelle.

## 12.6 En optimisé

En `-O2`, l'appel virtuel reste un appel indirect par la vtable, souvent condensé et transformé en *tail call* :

```asm
; calcule en -O2
mov rax, [rdi]        ; vptr
jmp [rax]             ; saute directement sur vtable[0] (tail call)
```

Le `jmp [rax]` au lieu de `call` est une optimisation d'appel terminal : `calcule` n'a plus rien à faire après, donc il saute dans `aire()` qui renverra directement à l'appelant de `calcule`. Le compilateur ne **dévirtualise** (remplace par un appel direct) que s'il peut prouver le type dynamique, ce qui n'est pas le cas ici puisque `f` peut pointer sur n'importe quelle sous-classe de `Forme`.

## Exercices gradués

Compiler avec `g++ -O0 ...` (et `-O2` quand demandé), puis `objdump -d -M intel` et `objdump -t`. Utiliser une petite hiérarchie : une base `Forme` avec `virtual int aire() const`, et une dérivée `Carre` qui la redéfinit.

**Exercice 12.1 (niveau 1)** Compiler la hiérarchie, lister les symboles avec `objdump -t`, et traduire trois symboles décorés avec `c++filt`. Lesquels correspondent à `calcule`, à `Carre::aire`, à la vtable de `Carre` ?

**Exercice 12.2 (niveau 2)** Désassembler `Carre::aire`. À quel offset lit-il le champ `cote` ? En déduire la disposition complète d'un objet `Carre` (vptr, `tag`, `cote`) et expliquer pourquoi `cote` n'est pas à l'offset 0.

**Exercice 12.3 (niveau 2)** Désassembler `calcule` en `-O0`. Repérer les trois instructions qui, à partir de `this`, mènent au `call` : chargement du vptr, chargement de l'entrée de vtable, appel. Expliquer chacune.

**Exercice 12.4 (niveau 2)** Trouver le symbole de la vtable de `Carre` (`objdump -t`, puis `c++filt`). Que contient cette table, et pourquoi deux classes différentes ont-elles des vtables différentes ?

**Exercice 12.5 (niveau 3)** Comparer `calcule` en `-O0` et `-O2`. Expliquer le `jmp [rax]` de la version `-O2` (tail call) et pourquoi le compilateur n'a pas remplacé l'appel virtuel par un appel direct.

**Exercice 12.6 (niveau 3, lecture)** Dans un binaire, on lit `mov rax, [rdi]` suivi de `call [rax+0x10]`. Qu'est-ce qui est appelé ? Combien de méthodes virtuelles précèdent celle-ci dans la vtable ?

Solutions : [solutions/partie1.md](../solutions/partie1.md#cpp-vtables)

## Suivi

- [ ] Partie 2, chapitre 12 (C++ et vtables) : lecture
- [ ] Partie 2, chapitre 12 (C++ et vtables) : exercices
