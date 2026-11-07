; somme3.asm : long somme3(long a, long b, long c), appelable depuis C
section .text
    global somme3
somme3:
    lea rax, [rdi+rsi]          ; rax = a + b
    add rax, rdx                ; rax = a + b + c
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
