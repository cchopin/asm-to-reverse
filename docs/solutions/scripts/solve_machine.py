#!/usr/bin/env python3
# Solveur du defi "machine" (VM). Inverse le programme par caractere :
# R = rol8( (c ^ 0x2A) + 5 )  =>  c = ( ror8(R) - 5 ) ^ 0x2A
def ror8(v, k): return ((v >> k) | (v << (8 - k))) & 0xFF
cible = [0xEA,0xA2,0x0B,0xA2,0xEA,0xF2,0xA2,0x19,0xE8]
mdp = "".join(chr(((ror8(R, 3) - 5) & 0xFF) ^ 0x2A) for R in cible)
print("mot de passe :", mdp)
