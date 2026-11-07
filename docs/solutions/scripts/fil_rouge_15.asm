; fil_rouge_15.asm : affiche argv[1] suivi d'un retour à la ligne, ou un message d'usage
; Assemblage : nasm -f elf64 fil_rouge_15.asm -o fr15.o && ld fr15.o -o fr15
section .data
    usage     db "usage : fr15 <nom>", 10
    l_usage   equ $ - usage
    fin_ligne db 10

section .text
    global _start
_start:
    cmp qword [rsp], 2          ; argc
    jne .usage
    mov rsi, [rsp+16]           ; argv[1]

    ; longueur de la chaîne : rdx = strlen(rsi)
    xor edx, edx
.longueur:
    cmp byte [rsi+rdx], 0
    je .afficher
    inc rdx
    jmp .longueur

.afficher:
    mov eax, 1                  ; write(1, argv[1], longueur)
    mov edi, 1
    syscall
    mov eax, 1                  ; write(1, "\n", 1)
    mov edi, 1
    lea rsi, [rel fin_ligne]
    mov edx, 1
    syscall
    xor edi, edi                ; code de retour 0
    jmp .quitter

.usage:
    mov eax, 1
    mov edi, 2                  ; descripteur 2 : sortie d'erreur
    lea rsi, [rel usage]
    mov edx, l_usage
    syscall
    mov edi, 1                  ; code de retour 1

.quitter:
    mov eax, 60
    syscall
