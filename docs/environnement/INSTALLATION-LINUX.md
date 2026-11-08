# Installer l'environnement sous Linux

Le cours utilise un Linux x86-64. Sous Linux, deux voies sont possibles :

- **Le conteneur du cours** (recommandé) : le même [Dockerfile](Dockerfile) que sur Mac. Il fournit exactement les outils et le compilateur (gcc 13) qui ont produit les listings du cours, quelle que soit votre distribution. Les adresses et les instructions que vous obtiendrez correspondront donc à celles du texte.
- **Une installation directe** des outils avec le gestionnaire de paquets de votre distribution (Debian ou Ubuntu, Fedora, Arch). C'est plus léger et plus rapide, mais votre version de gcc peut différer de celle du cours : la logique des listings reste la même, les adresses et quelques instructions peuvent changer.

Dans les deux cas, Ghidra s'installe directement sur la machine (section 5).

Vérifier d'abord l'architecture :

```bash
uname -m
```

Le cours suppose `x86_64`. Sur une machine ARM (`aarch64`, par exemple un Raspberry Pi ou un portable ARM), GDB ne peut pas déboguer du code x86-64 sans émulation complète : utiliser alors une machine virtuelle x86-64 émulée (QEMU, ou UTM), dans laquelle on suit ce guide.

## 1. Voie recommandée : le conteneur du cours

### Installer Docker

Debian ou Ubuntu :

```bash
sudo apt update
sudo apt install docker.io
```

Fedora (le paquet Docker de Fedora s'appelle `moby-engine`) :

```bash
sudo dnf install moby-engine
```

Arch :

```bash
sudo pacman -S docker
```

Puis, quelle que soit la distribution, démarrer le service et autoriser votre utilisateur à lancer des conteneurs sans `sudo` :

```bash
sudo systemctl enable --now docker
sudo usermod -aG docker "$USER"
```

Se déconnecter puis se reconnecter (ou redémarrer) pour que l'ajout au groupe `docker` prenne effet. Vérification :

```bash
docker run --rm ubuntu:24.04 uname -m    # doit afficher x86_64
```

Être membre du groupe `docker` donne en pratique les droits d'administrateur sur la machine. Sur un poste personnel, c'est l'usage courant ; sinon, garder `sudo` devant chaque commande `docker`.

### Construire et lancer l'environnement

Depuis le dossier du cours (le dossier `docs` du dépôt GitHub) :

```bash
./environnement/lab.sh
```

Le script est le même que sur Mac. Il construit l'image `reverse-lab` au premier lancement (quelques minutes, sans émulation sous Linux x86-64), puis ouvre un shell dans le conteneur. Le dossier du cours est partagé dans `/cours` : on peut éditer avec l'éditeur de son choix sur la machine et compiler dans le conteneur. Les options `--sans-reseau` (aucun accès réseau, à utiliser pour le coffre v5 et les cas de malware) et `--rebuild` (reconstruire l'image après une modification du Dockerfile) fonctionnent de la même façon ; le détail des options de `docker run` est donné à la section 4 du guide [Installation sur Mac](INSTALLATION-MAC.md#4-construire-et-lancer-lenvironnement).

Les fichiers créés dans `/cours` depuis le conteneur appartiennent à `root` sur la machine. Pour les récupérer à son nom en fin de séance : `sudo chown -R "$USER": .` depuis le dossier du cours.

## 2. Installation directe : Debian et Ubuntu

```bash
sudo apt update
sudo apt install build-essential gcc-multilib nasm gdb strace ltrace \
    binutils file xxd tcpdump zlib1g-dev upx-ucl \
    python3 python3-pip python3-venv git curl wget unzip
```

`gcc-multilib` permet de compiler en 32 bits (`gcc -m32`, chapitre 13 de la partie 2) et `zlib1g-dev` sert au packer de démonstration (chapitre 9 de la partie 3). Sur Ubuntu 24.04, on obtient gcc 13, le même que celui du cours. Les outils Python s'installent ensuite comme à la section 5.

## 3. Installation directe : Fedora

```bash
sudo dnf install gcc make glibc-devel.i686 libgcc.i686 nasm gdb strace ltrace \
    binutils file xxd tcpdump zlib-devel upx \
    python3 python3-pip git curl wget unzip
```

Les paquets `.i686` fournissent la bibliothèque C 32 bits nécessaire à `gcc -m32`.

## 4. Installation directe : Arch

```bash
sudo pacman -S base-devel nasm gdb strace ltrace binutils file tinyxxd \
    tcpdump zlib upx python python-pip git curl wget unzip
```

`tinyxxd` fournit la commande `xxd` (elle est aussi installée avec `vim`). Pour compiler en 32 bits, activer le dépôt `[multilib]` dans `/etc/pacman.conf` (décommenter les deux lignes de la section), puis :

```bash
sudo pacman -Sy lib32-glibc lib32-gcc-libs
```

## 5. Outils communs aux trois distributions

### Les outils Python

z3, YARA, pefile, pwntools, Frida et angr s'installent le plus simplement dans un environnement virtuel Python, ce qui évite les différences de noms de paquets entre distributions :

```bash
python3 -m venv ~/reverse-venv
source ~/reverse-venv/bin/activate
pip install z3-solver yara-python pefile pwntools frida-tools angr
```

La commande `source ~/reverse-venv/bin/activate` est à relancer dans chaque nouveau terminal (ou à ajouter à la fin de `~/.bashrc`). `angr` est volumineux : son installation prend plusieurs minutes.

### GEF, l'extension de GDB

```bash
wget -O ~/.gef.py https://raw.githubusercontent.com/hugsy/gef/main/gef.py
echo "source ~/.gef.py" >> ~/.gdbinit
echo "set disassembly-flavor intel" >> ~/.gdbinit
```

La dernière ligne fait afficher à GDB la syntaxe Intel, celle du cours.

### capa et FLOSS (chapitre 6 de la partie 3)

```bash
cd /tmp
wget https://github.com/mandiant/capa/releases/download/v7.0.1/capa-v7.0.1-linux.zip
wget https://github.com/mandiant/flare-floss/releases/download/v3.1.0/floss-v3.1.0-linux.zip
sudo unzip capa-v7.0.1-linux.zip -d /usr/local/bin
sudo unzip floss-v3.1.0-linux.zip -d /usr/local/bin
```

### ptrace et les permissions

Sur une installation directe, GDB, `strace` et `ltrace` fonctionnent sans réglage pour un programme qu'ils lancent eux-mêmes (`gdb ./prog`). En revanche, **s'attacher** à un processus déjà lancé (`gdb -p PID`, `strace -p PID`) est bloqué par défaut sur Ubuntu et d'autres distributions (protection Yama, `kernel.yama.ptrace_scope = 1`) : utiliser alors `sudo`. Le conteneur, lui, autorise `ptrace` grâce aux options passées par `lab.sh`.

## 6. Ghidra

Ghidra demande Java 21 :

| Distribution | Commande |
|---|---|
| Debian, Ubuntu | `sudo apt install openjdk-21-jdk` |
| Fedora | `sudo dnf install java-21-openjdk-devel` |
| Arch | `sudo pacman -S jdk21-openjdk` |

Télécharger ensuite l'archive sur https://ghidra-sre.org (ou depuis la page *Releases* du dépôt GitHub de Ghidra), la décompresser dans le dossier de son choix et lancer `./ghidraRun`. Sur Arch, Ghidra est aussi disponible directement : `sudo pacman -S ghidra`.

Binary Ninja (facultatif, voir la section 7 du guide Mac) propose aussi une version Free pour Linux.

## 7. Vérifier l'installation

Dans le conteneur, ou directement dans un terminal pour une installation directe (en remplaçant `/cours` par le chemin du dossier `docs` du cours) :

```bash
uname -m                                    # x86_64
gcc --version | head -1                     # gcc 13 dans le conteneur
cd /cours/fil-rouge/partie1
gcc -O0 -fno-stack-protector -no-pie -o mystere mystere.c
gdb -batch -ex 'break verifier' -ex 'run 12345678' -ex 'info registers rdi' ./mystere
```

La dernière commande doit s'arrêter dans `verifier` et afficher la valeur de `rdi`. Vérifier aussi le 32 bits et les outils Python :

```bash
echo 'int main(void){return 0;}' > t.c && gcc -m32 t.c -o t32 && file t32   # ELF 32-bit
python3 -c "import z3, yara, pefile; print('ok')"
```

Si `gcc -m32` échoue avec un message sur `bits/libc-header-start.h` ou `crt1.o`, il manque la bibliothèque C 32 bits (voir la section de votre distribution).

## Suivi

- [ ] Environnement Linux installé et vérifié
