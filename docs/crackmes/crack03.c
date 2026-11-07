#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    unsigned char att[7] = {0x72, 0x62, 0x78, 0x70, 0x6e, 0x50, 0x4f};
    if (argc != 2 || strlen(argv[1]) != 7) { printf("Refuse\n"); return 0; }
    for (int i = 0; i < 7; i++)
        if (((unsigned char)argv[1][i] ^ (i * 7)) != att[i]) { printf("Refuse\n"); return 0; }
    printf("Acces autorise\n");
    return 0;
}
