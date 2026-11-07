#!/usr/bin/env python3
# z3 sur crack05 (CTF maison) : 4 chiffres sous contraintes. Solution unique.
from z3 import Int, Solver, sat
d = [Int('d%d' % i) for i in range(4)]
s = Solver()
for x in d:
    s.add(x >= 0, x <= 9)
s.add(d[0] + d[1] + d[2] + d[3] == 20)
s.add(d[0] == d[3] + 1)
s.add(d[1] == d[2] * 2)
s.add(d[0] > d[1])
assert s.check() == sat
m = s.model()
print("code :", ''.join(str(m[x]) for x in d))   # -> 9218
