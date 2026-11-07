#!/usr/bin/env python3
# Entropie de Shannon (bits/octet) d'un fichier ou d'une section.
import sys, math, collections
data = open(sys.argv[1], "rb").read()
if not data: print("0.00"); sys.exit()
c = collections.Counter(data)
n = len(data)
h = -sum((v/n) * math.log2(v/n) for v in c.values())
print("%.2f bits/octet sur %d octets" % (h, n))
