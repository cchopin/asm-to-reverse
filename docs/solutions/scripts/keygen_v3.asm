; keygen.asm : générateur de serial du coffre v3, écrit en assembleur
; Assemblage : nasm -f elf64 keygen.asm -o keygen.o && gcc keygen.o -o keygen
; Utilisation : ./keygen <nom>

extern printf
extern puts

section .rodata
    format  db "%08X", 10, 0
    usage   db "usage : keygen <nom>", 0

section .text
    global main

; unsigned int hacher(const char *nom)   nom dans rdi, résultat dans eax
hacher:
    mov eax, 0x1337             ; h = 0x1337
.boucle:
    movzx edx, byte [rdi]       ; c = *nom
    test dl, dl
    jz .fin                     ; fin de chaîne
    imul eax, eax, 31           ; h = h * 31
    add eax, edx                ; h = h + c
    rol eax, 3                  ; rotation à gauche de 3 bits
    inc rdi                     ; caractère suivant
    jmp .boucle
.fin:
    xor eax, 0xC0FFEE
    ret

; int main(int argc, char **argv)
main:
    push rbx                    ; sauvegarde (et aligne rsp sur 16 octets)
    cmp edi, 2
    jne .usage
    mov rdi, [rsi+8]            ; argv[1]
    call hacher
    lea rdi, [rel format]       ; 1er argument de printf
    mov esi, eax                ; 2e argument : la valeur
    xor eax, eax                ; aucun registre vectoriel
    call printf wrt ..plt
    xor eax, eax                ; return 0
    pop rbx
    ret
.usage:
    lea rdi, [rel usage]
    call puts wrt ..plt
    mov eax, 1
    pop rbx
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
