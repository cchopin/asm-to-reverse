#!/bin/sh
# Lance l'environnement Linux x86-64 du cours.
#   ./environnement/lab.sh               shell dans le conteneur, dossier du cours monte dans /cours
#   ./environnement/lab.sh --sans-reseau  meme chose, sans aucun acces reseau
#   ./environnement/lab.sh --rebuild      reconstruit l'image avant de lancer
set -e
cd "$(dirname "$0")/.."

RESEAU=""
for arg in "$@"; do
    case "$arg" in
        --sans-reseau) RESEAU="--network none" ;;
        --rebuild) docker build --platform linux/amd64 -t reverse-lab environnement ;;
    esac
done

if ! docker image inspect reverse-lab >/dev/null 2>&1; then
    docker build --platform linux/amd64 -t reverse-lab environnement
fi

# SYS_PTRACE et seccomp=unconfined : necessaires pour GDB, strace et ltrace
exec docker run --rm -it --platform linux/amd64 \
    --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
    --hostname reverse-lab $RESEAU \
    -v "$PWD":/cours reverse-lab
