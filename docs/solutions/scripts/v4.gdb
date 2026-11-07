# Coffre v4 : solution dynamique (adresses valables pour gcc 13 -O1, a adapter).
# Utilisation : gdb -batch -x v4.gdb ./coffre_v4
set pagination off
# starti charge le programme (et la base PIE 0x555555554000 sous GDB)
starti
# juste apres "call ptrace@plt" (offset 0x11fb)
break *0x5555555551fb
# juste apres "call dechiffrer" (offset 0x12d7), avant l'effacement du flag
break *0x5555555552d7
continue
# ptrace a renvoye -1 car GDB trace deja le processus : on fait croire a un succes
set $rax = 0
continue
# rbx contient l'adresse du tampon dechiffre
x/s $rbx
continue
