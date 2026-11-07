#!/usr/bin/env python3
# Rejoue les regles du chapitre 22 sur les binaires de test.
# Prerequis : pip install yara-python ; construire demo, packed (ch.21) et specimen (capstone).
import sys, yara
rules = yara.compile(filepath="regles.yar")
for f in sys.argv[1:]:
    m = rules.match(f)
    noms = ", ".join(r.rule for r in m) if m else "(aucune)"
    print("%-12s -> %s" % (f, noms))
