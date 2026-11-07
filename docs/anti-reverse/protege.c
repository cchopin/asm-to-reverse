/* protege : plusieurs anti-debug combines. Affiche le flag seulement si
   aucun debogueur n'est detecte. Pedagogique : a contourner.
   Remarque : ptrace(TRACEME) est teste EN DERNIER, car l'appeler fait de
   notre parent un traceur et fausserait le test TracerPid suivant. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>

static int tracerpid_non_nul(void){
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return 0;
    char l[256]; int pid = 0;
    while (fgets(l, sizeof(l), f))
        if (strncmp(l, "TracerPid:", 10) == 0){ pid = atoi(l + 10); break; }
    fclose(f);
    return pid != 0;
}
static int ld_preload_present(void){ return getenv("LD_PRELOAD") != NULL; }
static unsigned long long rdtsc(void){
    unsigned hi, lo;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)hi << 32) | lo;
}
int main(void){
    if (tracerpid_non_nul()){ puts("debogueur detecte (TracerPid)"); return 1; }
    if (ld_preload_present()){ puts("LD_PRELOAD detecte"); return 1; }
    unsigned long long t0 = rdtsc();
    volatile int s = 0; for (int i = 0; i < 1000; i++) s += i;
    if (rdtsc() - t0 > 5000000ULL){ puts("execution trop lente (pas a pas ?)"); return 1; }
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0){ puts("debogueur detecte (ptrace)"); return 1; }
    puts("FLAG{anti_debug_contourne}");
    return 0;
}
