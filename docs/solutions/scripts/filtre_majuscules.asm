; filtre_majuscules.asm : met en majuscules tout ce qui arrive sur l'entree
; standard et le renvoie sur la sortie standard, sans libc.
; nasm -f elf64 filtre_majuscules.asm -o f.o && ld f.o -o filtre_majuscules
section .bss
    buf resb 4096
section .text
global _start
_start:
.lire:
    xor eax, eax                ; read(0, buf, 4096)
    xor edi, edi
    lea rsi, [rel buf]
    mov edx, 4096
    syscall
    test rax, rax
    jle .fin                    ; 0 = fin de fichier
    mov rcx, rax                ; nombre d'octets lus
    xor rbx, rbx                ; index
.boucle:
    mov dl, [buf + rbx]
    cmp dl, 'a'
    jb .suivant
    cmp dl, 'z'
    ja .suivant
    sub dl, 0x20                ; minuscule -> majuscule
    mov [buf + rbx], dl
.suivant:
    inc rbx
    cmp rbx, rcx
    jl .boucle
    ; write(1, buf, rcx)
    mov eax, 1
    mov edi, 1
    lea rsi, [rel buf]
    mov rdx, rcx
    syscall
    jmp .lire
.fin:
    mov eax, 60
    xor edi, edi
    syscall
section .note.GNU-stack noalloc noexec nowrite progbits
