# Règles YARA du chapitre 22

- `regles.yar` : les règles du chapitre (signature par chaîne, détection de
  format par magic, entropie/packing, IOC XORés du specimen, imports PE
  conceptuels).
- `tester.py` : rejoue les règles sur des fichiers passés en argument.

Installation : `pip install yara-python` (le module `yara` en Python), ou
l'outil en ligne de commande `yara` de votre distribution.

Exemple :

```
# construire les binaires de test (voir chapitres 21 et capstone)
python3 tester.py demo packed specimen
# demo     -> demo_reverse42, est_un_elf
# packed   -> est_un_elf
# specimen -> est_un_elf, specimen_campagne_orange
```

En ligne de commande :

```
yara regles.yar demo
yara -r regles.yar un_dossier/    # balayer recursivement un dossier
```
