#!/usr/bin/env python3
# Mini-packer pedagogique : compresse payload.bin (zlib) et emet le tableau C
# a coller dans packed.c. Imite le principe d'un vrai packer.
import zlib, sys
src = sys.argv[1] if len(sys.argv) > 1 else "payload.bin"
d = open(src, "rb").read()
c = zlib.compress(d, 9)
arr = ',\n    '.join(', '.join('0x%02x' % c[i+j] for j in range(min(12, len(c)-i)))
                      for i in range(0, len(c), 12))
print("// taille originale = %d" % len(d))
print("static const unsigned char blob[] = {\n    %s\n};" % arr)
