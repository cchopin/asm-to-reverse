#!/usr/bin/env python3
# Keygen du defi "serrure" (capstone). Rejoue la fonction melange.
import sys
def melange(nom):
    h = 0x5EED
    for i, c in enumerate(nom.encode()):
        h ^= c
        h = ((h << 4) | (h >> 28)) & 0xFFFFFFFF   # rol 4 sur 32 bits
        h = (h + 0x9E37 + i) & 0xFFFFFFFF
    return h & 0xFFFFFF
nom = sys.argv[1] if len(sys.argv) > 1 else "Clemence"
print("%06X" % melange(nom))
