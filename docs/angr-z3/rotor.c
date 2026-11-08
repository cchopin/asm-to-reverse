/* rotor.c : crackme du chapitre angr/z3 (partie 3, chapitre 12).
   Chaque caractere subit rol8((c ^ 0x5C) + 9, 2), puis est compare a une cible.
   gcc -O1 -o rotor rotor.c */
#include <stdio.h>
#include <string.h>

static const unsigned char cible[] = {
    0xE0,0xF0,0xE4,0xCC,0x09,0xC8,0xDC,0xDD,0xCD
};

static unsigned char rol8(unsigned char v, int k) { return (v << k) | (v >> (8 - k)); }

int main(int argc, char **argv)
{
    if (argc != 2 || strlen(argv[1]) != sizeof(cible)) { printf("Refuse\n"); return 1; }
    for (size_t i = 0; i < sizeof(cible); i++) {
        unsigned char v = rol8((unsigned char)((argv[1][i] ^ 0x5C) + 9), 2);
        if (v != cible[i]) { printf("Refuse\n"); return 1; }
    }
    printf("Acces autorise\n");
    return 0;
}
