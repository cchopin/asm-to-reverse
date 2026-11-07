#!/usr/bin/env python3
# z3 sur la VM du defi "machine" (capstone). Pour chaque caractere c :
# cible = rol8( (c ^ 0x2A) + 5, 3 ). On inverse avec un solveur de bitvectors.
from z3 import BitVec, Solver, RotateLeft, sat
cible = [0xEA, 0xA2, 0x0B, 0xA2, 0xEA, 0xF2, 0xA2, 0x19, 0xE8]
cs = [BitVec('c%d' % i, 8) for i in range(len(cible))]
s = Solver()
for i, c in enumerate(cs):
    v = RotateLeft((c ^ 0x2A) + 5, 3)
    s.add(v == cible[i])
    s.add(c >= 0x20, c <= 0x7e)            # caractere imprimable
assert s.check() == sat
m = s.model()
print("mot de passe :", ''.join(chr(m[c].as_long()) for c in cs))   # -> reverse42
