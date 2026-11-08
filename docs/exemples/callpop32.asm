; callpop32.asm : recuperer l'adresse d'une chaine sans adressage relatif a eip
; Assemblage : nasm -f elf32 callpop32.asm -o callpop32.o && ld -m elf_i386 callpop32.o -o callpop32
section .text
    global _start

_start:
    call apres
    db   "32 bits!", 10   ; donnees placees juste apres le call (9 octets)
apres:
    pop  ecx              ; ecx = adresse de la chaine (poussee par call)
    mov  edx, 9           ; longueur
    mov  ebx, 1           ; descripteur 1 : sortie standard
    mov  eax, 4           ; appel systeme write en 32 bits (numero 4)
    int  0x80

    xor  ebx, ebx         ; exit(0)
    mov  eax, 1
    int  0x80
