/* lanceur.c : charge un shellcode depuis un fichier et l'exécute
 * Compilation : gcc lanceur.c -o lanceur
 * Utilisation : ./lanceur shellcode.bin
 */
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage : lanceur <fichier>\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        perror("fopen");
        return 1;
    }

    /* une page mémoire lisible, modifiable et exécutable */
    unsigned char *code = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (code == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    size_t n = fread(code, 1, 4096, f);
    fclose(f);
    printf("%zu octets charges en %p\n", n, (void *)code);
    fflush(stdout);

    /* convertir l'adresse en pointeur de fonction, puis l'appeler */
    void (*shellcode)(void) = (void (*)(void))code;
    shellcode();
    return 0;
}
