# Règles YARA de la partie 3, chapitre 14

- `regles.yar` : les règles du chapitre (signature par chaîne, détection de
  format par magic, entropie/packing, IOC XORés de coffre_v5, imports PE
  conceptuels).
- `tester.py` : rejoue les règles sur des fichiers passés en argument.

Installation : `pip install yara-python` (le module `yara` en Python), ou
l'outil en ligne de commande `yara` de votre distribution.

Exemple :

```
# construire les binaires de test (voir chapitres 13 et 6)
python3 tester.py demo packed coffre_v5
# demo     -> demo_reverse42, est_un_elf
# packed   -> est_un_elf
# coffre_v5 -> est_un_elf, coffre_v5_campagne_rouge
```

En ligne de commande :

```
yara regles.yar demo
yara -r regles.yar un_dossier/    # balayer recursivement un dossier
```
