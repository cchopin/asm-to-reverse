# Environnement Linux x86-64 pour le cours de reverse.
# Ubuntu 24.04 : meme compilateur (gcc 13) que celui utilise pour les listings du cours.
#
# Construction (depuis ce dossier) :
#   docker build --platform linux/amd64 -t reverse-lab .
# Voir INSTALLATION-MAC.md pour le lancement.

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential gcc-multilib nasm \
        gdb strace ltrace \
        binutils file xxd bsdextrautils tcpdump \
        python3 python3-pip python3-venv \
        python3-z3 python3-pefile yara python3-yara \
        upx-ucl \
        git curl wget unzip ca-certificates less nano vim procps \
    && rm -rf /var/lib/apt/lists/*

# pwntools : ecriture de shellcode et analyse (partie 3)
RUN pip3 install --no-cache-dir --break-system-packages pwntools

# GEF : extension de GDB (installation manuelle documentee par le projet)
RUN wget -q -O /root/.gef.py https://raw.githubusercontent.com/hugsy/gef/main/gef.py \
    && echo "source /root/.gef.py" > /root/.gdbinit \
    && echo "set disassembly-flavor intel" >> /root/.gdbinit

# capa et FLOSS (Mandiant) : capacites d'un binaire et extraction de chaines (chapitre 13)
RUN cd /tmp \
    && wget -q https://github.com/mandiant/capa/releases/download/v7.0.1/capa-v7.0.1-linux.zip \
    && wget -q https://github.com/mandiant/flare-floss/releases/download/v3.1.0/floss-v3.1.0-linux.zip \
    && unzip -q capa-v7.0.1-linux.zip -d /usr/local/bin \
    && unzip -q floss-v3.1.0-linux.zip -d /usr/local/bin \
    && chmod +x /usr/local/bin/capa /usr/local/bin/floss \
    && rm -f /tmp/*.zip

# Petit message d'accueil
RUN echo 'echo "reverse-lab : $(uname -m), gcc $(gcc -dumpversion). Dossier du cours : /cours"' >> /root/.bashrc

WORKDIR /cours
CMD ["/bin/bash"]
