#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 2 || strlen(argv[1]) != 4) { printf("Refuse\n"); return 0; }
    int d[4];
    for (int i = 0; i < 4; i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') { printf("Refuse\n"); return 0; }
        d[i] = argv[1][i] - '0';
    }
    if (d[0] + d[1] + d[2] + d[3] == 20 && d[0] == d[3] + 1 && d[1] == d[2] * 2 && d[0] > d[1])
        printf("Coffre ouvert\n");
    else
        printf("Refuse\n");
    return 0;
}
