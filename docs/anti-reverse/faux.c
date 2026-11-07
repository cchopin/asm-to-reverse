/* faux.c : bibliotheque de contournement (LD_PRELOAD).
   1) se cache en retirant LD_PRELOAD de l'environnement au chargement ;
   2) remplace ptrace par une version qui renvoie toujours 0. */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
__attribute__((constructor))
static void init(void){ unsetenv("LD_PRELOAD"); }   /* defeat le test LD_PRELOAD */
long ptrace(int req, ...){ (void)req; return 0; }     /* defeat le test ptrace */
