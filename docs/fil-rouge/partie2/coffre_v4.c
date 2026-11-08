/* Coffre v4 : le coffre se defend (partie 3, chapitre 5).
 * Compilation : gcc -O1 -o coffre_v4 coffre_v4.c && strip coffre_v4
 * Objectif : lire le flag, qui n'existe en clair qu'en memoire,
 * pendant une fraction de seconde. Il faut d'abord contourner
 * les protections anti-debug.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <time.h>

/* Flag chiffre avec une cle tournante (voir dechiffrer) */
static unsigned char secret[] = {
    0x1f, 0x8c, 0x82, 0x95, 0x66, 0xf5, 0x89, 0xf2, 0xc8, 0x77,
    0xaf, 0x9f, 0x87, 0x09, 0x08, 0x71, 0x9f, 0xf1, 0xba, 0x4c,
    0xce, 0x11, 0x8a
};

static int surveille_ptrace(void)
{
    /* Un processus ne peut etre trace qu'une seule fois. Si gdb est
       deja attache, cet appel echoue. */
    return ptrace(PTRACE_TRACEME, 0, 0, 0) == -1;
}

static int surveille_temps(void)
{
    struct timespec a, b;
    clock_gettime(CLOCK_MONOTONIC, &a);
    volatile unsigned int x = 0;
    for (int i = 0; i < 1000000; i++)
        x += i;
    clock_gettime(CLOCK_MONOTONIC, &b);
    long ms = (b.tv_sec - a.tv_sec) * 1000 + (b.tv_nsec - a.tv_nsec) / 1000000;
    return ms > 500;   /* trop lent : quelqu'un fait du pas a pas */
}

__attribute__((noinline)) static void dechiffrer(unsigned char *buf, size_t n)
{
    unsigned char k = 0x59;
    for (size_t i = 0; i < n; i++) {
        buf[i] ^= k;
        k = (unsigned char)(k * 5 + 3);
    }
}

int main(void)
{
    if (surveille_ptrace() || surveille_temps()) {
        puts("Je te vois. Le coffre reste ferme.");
        return 1;
    }

    unsigned char flag[sizeof(secret) + 1];
    memcpy(flag, secret, sizeof(secret));
    dechiffrer(flag, sizeof(secret));
    flag[sizeof(secret)] = 0;

    /* Le flag est en memoire, mais on ne l'affiche pas ! */
    __asm__ volatile("" : : "r"(flag) : "memory");
    memset(flag, 0, sizeof(flag));
    __asm__ volatile("" : : "r"(flag) : "memory");

    puts("Le coffre s'est ouvert puis referme. Le flag est passe en memoire, sans etre affiche.");
    return 0;
}
