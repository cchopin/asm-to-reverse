#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
int main(int argc, char **argv)
{
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
        printf("Debugger detecte.\n");
        return 1;
    }
    if (argc == 2 && strcmp(argv[1], "antidebug") == 0)
        printf("Bravo\n");
    else
        printf("Refuse\n");
    return 0;
}
