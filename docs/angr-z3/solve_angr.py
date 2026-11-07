#!/usr/bin/env python3
# angr sur un crackme (a executer sur votre machine : pip install angr).
# Explore automatiquement le binaire jusqu'a l'adresse du succes.
import angr, sys
proj = angr.Project(sys.argv[1], auto_load_libs=False)
simgr = proj.factory.simulation_manager(proj.factory.full_init_state())
# Adresses a relever dans le desassemblage :
ADR_SUCCES = 0x401180   # bloc qui affiche "Coffre ouvert" / le flag
ADR_ECHEC  = 0x4011a0   # bloc qui affiche "Refuse"
simgr.explore(find=ADR_SUCCES, avoid=ADR_ECHEC)
if simgr.found:
    etat = simgr.found[0]
    print("entree trouvee :", etat.posix.dumps(0))   # ce qui a ete lu sur stdin
else:
    print("aucun chemin vers le succes")
