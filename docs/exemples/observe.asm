; observe.asm : quelques instructions à exécuter pas à pas dans GDB
; Assemblage : nasm -f elf64 -g observe.asm -o observe.o && ld observe.o -o observe
section .text
    global _start

_start:
    mov eax, 5          ; eax = 5
    mov ebx, eax        ; ebx = 5
    add eax, 4          ; eax = 9
    imul eax, ebx       ; eax = 45
    sub eax, 5          ; eax = 40
    lea ecx, [rax+rax*2]; ecx = 120, eax n'est pas modifié
    xor edx, edx        ; edx = 0, et ZF passe à 1
    cmp eax, 40         ; eax - 40 = 0 : ZF = 1
    cmp eax, 50         ; eax - 50 < 0 : SF = 1 et CF = 1
    mov rax, -1         ; rax = 0xffffffffffffffff
    mov al, 0x41        ; seul l'octet bas change : rax = 0xffffffffffffff41
    mov eax, 1          ; écrire eax met à zéro le haut de rax : rax = 1

    mov eax, 60         ; exit(0) : appel système, voir chapitre 15
    xor edi, edi
    syscall
