#include <stdio.h>
#include <string.h>
/* Verifie un serial derive du nom. Compilation :
   gcc -O1 -fno-stack-protector -no-pie serrure.c -o serrure */
static unsigned int melange(const char *nom)
{
    unsigned int h = 0x5EED;
    for (int i = 0; nom[i]; i++) {
        h ^= (unsigned char)nom[i];
        h = (h << 4) | (h >> 28);   /* rol 4 */
        h += 0x9E37 + i;
    }
    return h & 0x00FFFFFF;          /* 24 bits -> 6 chiffres hex */
}
int main(int argc, char **argv)
{
    if (argc != 3) { printf("usage: %s <nom> <serial-hex>\n", argv[0]); return 2; }
    unsigned int attendu = melange(argv[1]);
    unsigned int donne = 0;
    if (sscanf(argv[2], "%x", &donne) != 1) { printf("serial invalide\n"); return 1; }
    if (donne == attendu) printf("FLAG{serrure_%06X}\n", attendu);
    else printf("Refuse\n");
    return 0;
}
