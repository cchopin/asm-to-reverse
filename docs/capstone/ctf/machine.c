#include <stdio.h>
#include <string.h>
/* Mini-VM : pour chaque caractere du mot de passe, execute le meme petit
   programme d'octets, puis compare le registre au tableau cible.
   gcc -O1 -fno-stack-protector -no-pie machine.c -o machine */

/* programme par caractere : opcode, operande ... 0xFF = fin */
static const unsigned char prog[] = {
    0x02, 0x2A,   /* R ^= 0x2A            */
    0x01, 0x05,   /* R += 5               */
    0x03, 0x03,   /* R = rol8(R, 3)       */
    0xFF
};
/* cible calculee pour le bon mot de passe */
static const unsigned char cible[] = {
    0xEA,0xA2,0x0B,0xA2,0xEA,0xF2,0xA2,0x19,0xE8
};

static unsigned char rol8(unsigned char v, int k){ return (v<<k)|(v>>(8-k)); }

static int verifie(const char *mdp)
{
    if (strlen(mdp) != sizeof(cible)) return 0;
    for (size_t i = 0; i < sizeof(cible); i++) {
        unsigned char R = (unsigned char)mdp[i];
        for (size_t p = 0; prog[p] != 0xFF; p += 2) {
            unsigned char op = prog[p], arg = prog[p+1];
            if (op == 0x01) R += arg;
            else if (op == 0x02) R ^= arg;
            else if (op == 0x03) R = rol8(R, arg);
            else if (op == 0x04) R -= arg;
        }
        if (R != cible[i]) return 0;
    }
    return 1;
}
int main(int argc, char **argv)
{
    if (argc != 2) { printf("usage: %s <mot-de-passe>\n", argv[0]); return 2; }
    if (verifie(argv[1])) printf("FLAG{vm_%s}\n", argv[1]);
    else printf("Refuse\n");
    return 0;
}
