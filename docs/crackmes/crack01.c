#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "sesame") == 0)
        printf("Acces autorise\n");
    else
        printf("Refuse\n");
    return 0;
}
