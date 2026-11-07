#!/usr/bin/env python3
# Neutralise le test TracerPid de 'protege' en remplacant son saut
# conditionnel (jne) par des NOP. Produit 'protege_patch'.
b = bytearray(open("protege", "rb").read())
pat = bytes([0x85, 0xc0, 0x0f, 0x85, 0x8b, 0x00, 0x00, 0x00])  # test eax,eax ; jne ...
i = b.find(pat)
assert i != -1 and b.find(pat, i + 1) == -1, "motif introuvable ou non unique"
for k in range(2, 8):
    b[i + k] = 0x90           # jne -> 6 NOP
open("protege_patch", "wb").write(b)
import os; os.chmod("protege_patch", 0o755)
print("TracerPid neutralise a l'offset", hex(i))
