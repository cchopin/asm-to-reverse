# CTF maison

Une série de six crackmes de difficulté croissante, à résoudre avec la méthode en quatre étapes (reconnaître, observer, comprendre, agir). Chacun exerce une technique vue dans la partie 3. C'est l'entraînement le plus proche d'un vrai CTF.

!!! warning "Règle du jeu"
    Les sources sont fournies uniquement pour **compiler** les binaires. Les ouvrir avant d'avoir résolu le défi supprime tout l'intérêt. Résolvez sur le binaire (`objdump`, Ghidra, GDB), puis, seulement ensuite, comparez avec le source pour vérifier votre compréhension.

## Mise en place

Télécharger les sources et compiler (dans le conteneur) :

- [crack01.c](../crackmes/crack01.c), [crack02.c](../crackmes/crack02.c), [crack03.c](../crackmes/crack03.c), [crack04.c](../crackmes/crack04.c), [crack05.c](../crackmes/crack05.c), [crack06.c](../crackmes/crack06.c), [Makefile](../crackmes/Makefile)

```bash
make            # compile les six crackmes
```

Pour corser le défi, vous pouvez les stripper (`strip crackNN`) afin de perdre les noms de symboles.

## Les défis

### Défi 1 - crack01 (N1)

- **Binaire :** `crack01`
- **À produire :** le mot de passe accepté

Le programme attend un mot de passe en argument et répond `Acces autorise` ou `Refuse`. Indice : parfois, `strings` suffit.

### Défi 2 - crack02 (N1)

- **Binaire :** `crack02`
- **À produire :** le mot de passe accepté

Même principe, mais le mot de passe n'apparaît plus en clair. Observez comment chaque caractère saisi est transformé avant d'être comparé, et inversez la transformation.

### Défi 3 - crack03 (N2)

- **Binaire :** `crack03`
- **À produire :** la clé acceptée

La transformation dépend cette fois de la position du caractère. Retrouvez la règle, puis fabriquez la clé position par position.

### Défi 4 - crack04 (N2)

- **Binaire :** `crack04` (usage `./crack04 <nom> <serial>`)
- **Fichier à rendre :** un keygen
- **À produire :** un serial valide pour n'importe quel nom

Le serial est calculé à partir du nom. Comprenez le calcul, puis écrivez un keygen.

### Défi 5 - crack05 (N3)

- **Binaire :** `crack05`
- **À produire :** le code à quatre chiffres accepté

Un code soumis à plusieurs contraintes. Récupérez-les dans le code, puis résolvez-les (à la main ou avec z3, voir chapitre 4). Une seule combinaison fonctionne.

### Défi 6 - crack06 (N3)

- **Binaire :** `crack06`
- **À produire :** le mot de passe, et la façon de neutraliser l'anti-debug

Un crackme protégé contre le débogage : sous GDB, il refuse de coopérer. Trouvez d'abord le mot de passe par analyse statique, puis neutralisez l'anti-debug pour l'observer en dynamique (voir chapitre 5).

## Vérifier

Un défi est résolu quand le binaire accepte votre entrée, pas quand vous « pensez » avoir la réponse. Lancez-le pour confirmer.

Solutions détaillées : [solutions/partie2.md](../solutions/partie2.md#ctf-maison).

## Suivi

- [ ] CTF maison : défis 1 à 3
- [ ] CTF maison : défis 4 à 6
