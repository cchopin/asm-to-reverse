; desaligne.asm : appel de printf avec une pile mal alignée
; Assemblage : nasm -f elf64 desaligne.asm -o desaligne.o && gcc desaligne.o -o desaligne
extern printf

section .rodata
    format db "pi vaut environ %f", 10, 0
    pi     dq 3.14159

section .text
    global main
main:
    ; volontairement, aucun push ni sub rsp ici : rsp n'est pas aligné sur 16
    lea rdi, [rel format]
    movsd xmm0, [rel pi]        ; un nombre à virgule se passe dans xmm0
    mov eax, 1                  ; un registre vectoriel utilisé
    call printf wrt ..plt
    xor eax, eax
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
