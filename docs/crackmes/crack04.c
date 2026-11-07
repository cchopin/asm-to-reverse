#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
    if (argc != 3) { printf("Usage : %s <nom> <serial>\n", argv[0]); return 1; }
    long s = 0;
    for (int i = 0; argv[1][i]; i++) s += (unsigned char)argv[1][i];
    long serial = (s * 1337) % 100000;
    if (atol(argv[2]) == serial) printf("Licence valide\n");
    else printf("Licence invalide\n");
    return 0;
}
