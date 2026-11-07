; flag.asm : shellcode qui affiche un flag, sans octet nul ni adresse absolue
; Assemblage : nasm -f elf64 flag.asm -o flag.o && ld flag.o -o flag
section .text
    global _start
_start:
    ; construire la chaîne sur la pile, du dernier bloc au premier
    mov rax, 0x5858585858580a7d ; "}\nXXXXXX" (les X ne sont pas affichés)
    push rax
    mov rax, 0x6572666669686365 ; "echiffre"
    push rax
    mov rax, 0x645f65646f636c6c ; "llcode_d"
    push rax
    mov rax, 0x6568737b47414c46 ; "FLAG{she"
    push rax

    ; write(1, rsp, 26)
    xor eax, eax
    inc eax                     ; eax = 1 (write), sans octet nul
    mov edi, eax                ; edi = 1 (sortie standard)
    mov rsi, rsp                ; la chaîne est au sommet de la pile
    push 26
    pop rdx                     ; rdx = 26, encodé en 3 octets sans nul
    syscall

    ; exit(0)
    push 60
    pop rax
    xor edi, edi
    syscall
