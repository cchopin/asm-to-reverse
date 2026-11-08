#!/usr/bin/env python3
# Rejoue les regles de la partie 3, chapitre 14 sur les binaires de test.
# Prerequis : yara-python ; construire demo, packed (chapitre 13) et coffre_v5 (chapitre 6).
import sys, yara
rules = yara.compile(filepath="regles.yar")
for f in sys.argv[1:]:
    m = rules.match(f)
    noms = ", ".join(r.rule for r in m) if m else "(aucune)"
    print("%-12s -> %s" % (f, noms))
