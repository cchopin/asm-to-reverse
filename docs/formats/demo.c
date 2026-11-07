#include <stdio.h>
#include <string.h>
int secret(const char *s){ return strcmp(s, "reverse42") == 0; }
int main(int argc, char **argv){
    if (argc > 1 && secret(argv[1])) { puts("ok"); return 0; }
    puts("non"); return 1;
}
