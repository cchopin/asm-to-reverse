; chaines.asm : instructions de chaînes, loop et xchg, à observer dans GDB
; Assemblage : nasm -f elf64 -g chaines.asm -o chaines.o && ld chaines.o -o chaines
section .data
    source  db "reverse", 0
section .bss
    tampon  resb 16

section .text
    global _start
_start:
    ; strlen(source) avec repne scasb
    lea rdi, [rel source]   ; chaîne à parcourir
    xor eax, eax            ; al = octet cherché (0, la fin de chaîne)
    mov rcx, -1             ; compteur "infini"
    repne scasb             ; avance rdi et décrémente rcx tant que [rdi] != al
    not rcx                 ; rcx = longueur + 1
    dec rcx                 ; rcx = 7

    ; memset(tampon, 'A', 4) avec rep stosb
    lea rdi, [rel tampon]
    mov al, 'A'
    mov ecx, 4
    rep stosb               ; écrit al en [rdi], rcx fois

    ; memcpy(tampon + 4, source, 8) avec rep movsb
    lea rsi, [rel source]
    lea rdi, [rel tampon+4]
    mov ecx, 8
    rep movsb               ; copie [rsi] vers [rdi], rcx fois

    ; somme 1 + 2 + ... + 5 avec loop
    xor eax, eax
    mov ecx, 5
.somme:
    add eax, ecx
    loop .somme             ; rcx = rcx - 1, puis saute si rcx != 0

    ; échange avec xchg
    mov ebx, 1
    mov edx, 2
    xchg ebx, edx           ; ebx = 2, edx = 1

    mov eax, 60
    xor edi, edi
    syscall
