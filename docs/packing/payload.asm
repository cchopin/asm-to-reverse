section .text
global _start
_start:
    jmp short .a
.c:
    pop rsi
    mov eax, 1
    mov edi, 1
    mov edx, msglen
    syscall
    ret
.a:
    call .c
    msg db "code original execute: FLAG{depack_ok}", 10
    msglen equ $ - msg
