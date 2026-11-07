# Sources du chapitre « Lire les formats : ELF et PE »

- `demo.c` : petit programme C pour pratiquer `readelf`/`objdump` (contient une
  fonction `secret` et une chaîne `reverse42`, utile pour les exercices ELF et
  pour le contraste avec un binaire packé).

Compilation :

```
gcc -O0 -no-pie demo.c -o demo
readelf -h demo        # en-tete, point d'entree
readelf -l demo        # segments (chargement)
readelf -S demo        # sections (lien/analyse)
readelf -d demo        # bibliotheques NEEDED
readelf -s demo        # symboles (disparaissent apres strip)
```

Pour le packing (point d'entrée qui bascule dans le stub, chaîne qui disparaît),
voir le dépaqueteur zlib du chapitre 9 (partie 3) dans `docs/packing/`.
