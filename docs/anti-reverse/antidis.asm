section .text
global _start
_start:
    jmp court
    db 0xE8                  ; leurre : lu comme 'call rel32' par un desassembleur lineaire
court:
    mov eax, 60
    mov edi, 7
    syscall
