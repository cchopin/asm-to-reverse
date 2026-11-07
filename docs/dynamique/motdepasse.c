#include <stdio.h>
#include <string.h>
int main(int argc, char **argv){
    if (argc != 2){ printf("usage: %s <mot>\n", argv[0]); return 2; }
    if (strcmp(argv[1], "sX3-reverse!") == 0) printf("FLAG{ltrace_strcmp}\n");
    else printf("Refuse\n");
    return 0;
}
