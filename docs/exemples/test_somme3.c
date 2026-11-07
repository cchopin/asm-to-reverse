#include <stdio.h>

long somme3(long a, long b, long c);   /* définie dans somme3.asm */

int main(void) {
    printf("%ld\n", somme3(10, 20, 12));
    return 0;
}
