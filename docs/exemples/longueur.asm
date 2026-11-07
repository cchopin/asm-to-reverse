; longueur.asm : une fonction appelée depuis _start, sans libc
; Assemblage : nasm -f elf64 longueur.asm -o longueur.o && ld longueur.o -o longueur
section .data
    texte db "reverse engineering", 0

section .text
    global _start

; size_t longueur(const char *s)   s dans rdi, résultat dans rax
longueur:
    xor eax, eax                ; compteur = 0
.boucle:
    cmp byte [rdi+rax], 0       ; s[compteur] == 0 ?
    je .fin
    inc rax
    jmp .boucle
.fin:
    ret

_start:
    lea rdi, [rel texte]        ; 1er argument
    call longueur               ; rax = 19
    mov edi, eax                ; code de retour = longueur
    mov eax, 60
    syscall
