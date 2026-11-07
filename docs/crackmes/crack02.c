#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    unsigned char att[6] = {0x37, 0x33, 0x28, 0x35, 0x33, 0x28};
    if (argc != 2 || strlen(argv[1]) != 6) { printf("Refuse\n"); return 0; }
    for (int i = 0; i < 6; i++)
        if (((unsigned char)argv[1][i] ^ 0x5A) != att[i]) { printf("Refuse\n"); return 0; }
    printf("Acces autorise\n");
    return 0;
}
