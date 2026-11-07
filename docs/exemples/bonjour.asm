; bonjour.asm : afficher un message puis quitter, sans libc
section .data
    message db "Bonjour depuis l'assembleur", 10   ; 10 = '\n'
    longueur equ $ - message                       ; calculé par NASM

section .text
    global _start

_start:
    mov rax, 1          ; numéro de l'appel système write
    mov rdi, 1          ; 1er argument : descripteur 1 (sortie standard)
    mov rsi, message    ; 2e argument : adresse du texte
    mov rdx, longueur   ; 3e argument : nombre d'octets
    syscall

    mov rax, 60         ; numéro de l'appel système exit
    xor rdi, rdi        ; code de retour 0
    syscall
