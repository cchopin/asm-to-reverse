# Keygen du coffre v3 : python3 keygen_v3.py <nom>
import sys
def keygen(nom):
    h = 0x1337
    for c in nom.encode():
        h = (h * 31 + c) & 0xFFFFFFFF
        h = ((h << 3) | (h >> 29)) & 0xFFFFFFFF
    return h ^ 0xC0FFEE
print("%08X" % keygen(sys.argv[1]))
