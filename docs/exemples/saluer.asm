; saluer.asm : lit un nom sur l'entrée standard et répond "Bonjour <nom>"
; Assemblage : nasm -f elf64 saluer.asm -o saluer.o && ld saluer.o -o saluer
section .data
    debut     db "Bonjour "
    l_debut   equ $ - debut

section .bss
    nom       resb 64           ; tampon de 64 octets non initialisé

section .text
    global _start
_start:
    ; n = read(0, nom, 64)
    mov eax, 0                  ; numéro de read
    mov edi, 0                  ; descripteur 0 : entrée standard
    lea rsi, [rel nom]          ; adresse du tampon
    mov edx, 64                 ; taille maximale
    syscall                     ; rax = nombre d'octets lus
    mov r12, rax                ; r12 n'est pas modifié par syscall

    ; write(1, debut, l_debut)
    mov eax, 1
    mov edi, 1                  ; descripteur 1 : sortie standard
    lea rsi, [rel debut]
    mov edx, l_debut
    syscall

    ; write(1, nom, n)
    mov eax, 1
    mov edi, 1
    lea rsi, [rel nom]
    mov rdx, r12
    syscall

    ; exit(0)
    mov eax, 60
    xor edi, edi
    syscall
