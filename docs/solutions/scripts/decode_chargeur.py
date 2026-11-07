# decode_chargeur.py : extraction statique du shellcode chiffré de chargeur
# Utilisation : python3 decode_chargeur.py chargeur
import sys
from pwn import context, disasm

context.arch = "amd64"
OFFSET = 0x2020   # adresse du tableau, lue dans lea rcx,[rip+0xe33]
TAILLE = 0x41     # borne de la boucle : cmp rax,0x41
CLE = 0xA7        # xor edx,0xffffffa7 : seul l'octet bas compte

donnees = open(sys.argv[1], "rb").read()[OFFSET:OFFSET + TAILLE]
shellcode = bytes(b ^ CLE for b in donnees)
open("charge.bin", "wb").write(shellcode)
print(disasm(shellcode))
