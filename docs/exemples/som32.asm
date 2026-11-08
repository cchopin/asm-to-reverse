; som32.asm : int som(int a, int b) appelee en cdecl (x86 32 bits)
; Assemblage : nasm -f elf32 som32.asm -o som32.o && ld -m elf_i386 som32.o -o som32
; Execution : ./som32 ; echo $?     (affiche 42, le code de retour)
section .text
    global _start

_start:
    push 2              ; 2e argument (b), pousse en premier : droite a gauche
    push 40             ; 1er argument (a)
    call som
    add  esp, 8         ; cdecl : l'appelant retire ses 2 arguments (2 * 4 octets)

    mov  ebx, eax       ; code de retour = resultat de som
    mov  eax, 1         ; appel systeme exit en 32 bits (numero 1)
    int  0x80

som:
    push ebp
    mov  ebp, esp
    mov  eax, [ebp+8]   ; 1er argument (a)
    add  eax, [ebp+0xc] ; 2e argument (b)
    pop  ebp
    ret
