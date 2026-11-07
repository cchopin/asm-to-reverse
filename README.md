# Cours d'assembleur et de reverse engineering

Cours progressif en français, destiné aux débutants : lecture de l'assembleur x86-64 produit par un compilateur C, puis méthode de reverse engineering appliquée aux crackmes, aux CTF et à l'initiation à l'analyse de malware.

- **Partie 1** (chapitres 1 à 7) : registres, instructions, sauts, boucles, pile et fonctions, mémoire, idiomes du compilateur.
- **Partie 2** (chapitres 8 à 14) : méthode en quatre étapes, Ghidra, GDB, schémas classiques des crackmes, protections, analyse de malware.
- **Partie 3** (chapitres 15 à 17) : écrire de l'assembleur avec NASM, appels système, appel de la libc, shellcode et loader chiffré.
- **Trois exercices fil rouge** : une fonction à décompiler à la main (partie 1), un programme "Coffre" qui évolue de la v1 à la v5 (partie 2), et un keygen puis un loader à écrire et analyser en assembleur (partie 3).
- **Solutions séparées**, avec scripts testés.

## Lire le cours

- Sur le site (recommandé) : les schémas sont rendus et la progression se coche, chapitre par chapitre, avec mémorisation dans le navigateur.
- Directement sur GitHub : commencer par [docs/README.md](docs/README.md).

## Environnement

Un Linux x86-64 est nécessaire. Le dossier [docs/environnement](docs/environnement) fournit un Dockerfile (Ubuntu 24.04, gcc 13, GDB et GEF, strace, ltrace, Python, z3, pwntools, YARA, capa, FLOSS) et un script de lancement. Le guide [Installation sur Mac](docs/environnement/INSTALLATION-MAC.md) couvre les Mac Intel et Apple Silicon.

```bash
./docs/environnement/lab.sh
```

## Générer le site localement

```bash
pip install -r requirements.txt
mkdocs serve
```

Le site est publié automatiquement sur GitHub Pages à chaque modification de la branche `main` (voir `.github/workflows/pages.yml`).
