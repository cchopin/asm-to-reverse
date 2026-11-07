; detecte.asm : anti-debug par appel système direct à ptrace
; Assemblage : nasm -f elf64 detecte.asm -o detecte.o && ld detecte.o -o detecte
section .data
    ok        db "Aucun debogueur", 10
    l_ok      equ $ - ok
    trouve    db "Debogueur detecte", 10
    l_trouve  equ $ - trouve

section .text
    global _start
_start:
    ; ptrace(PTRACE_TRACEME, 0, 0, 0)
    mov eax, 101                ; numéro de ptrace
    xor edi, edi                ; PTRACE_TRACEME = 0
    xor esi, esi
    xor edx, edx
    xor r10d, r10d              ; 4e argument : r10, et non rcx
    syscall
    test rax, rax
    js .detecte                 ; résultat négatif : erreur, déjà tracé

    lea rsi, [rel ok]
    mov edx, l_ok
    jmp .afficher
.detecte:
    lea rsi, [rel trouve]
    mov edx, l_trouve
.afficher:
    mov eax, 1
    mov edi, 1
    syscall

    mov eax, 60
    xor edi, edi
    syscall
