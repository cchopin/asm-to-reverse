#!/usr/bin/env python3
# z3 sur rotor.c : pour chaque caractere c, cible = rol8((c ^ 0x5C) + 9, 2).
# On decrit le sens direct ; z3 trouve l'antecedent sans qu'on inverse a la main.
from z3 import BitVec, Solver, RotateLeft, sat
cible = [0xE0, 0xF0, 0xE4, 0xCC, 0x09, 0xC8, 0xDC, 0xDD, 0xCD]
cs = [BitVec('c%d' % i, 8) for i in range(len(cible))]
s = Solver()
for i, c in enumerate(cs):
    v = RotateLeft((c ^ 0x5C) + 9, 2)
    s.add(v == cible[i])
    s.add(c >= 0x20, c <= 0x7e)            # caractere imprimable
assert s.check() == sat
m = s.model()
print("mot de passe :", ''.join(chr(m[c].as_long()) for c in cs))   # -> solveur26
