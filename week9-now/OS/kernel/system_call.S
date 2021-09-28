section .data
str_msg1: db "c language call", 0xa
str_msg1_len equ $-str_msg1
str_msg2: db "system call", 0xa
str_msg2_len equ $-str_msg2

section .code
global _start
_start:
    push str_msg1_len
    push str_msg1
    push 1
    call self_write
    add esp, 12

    mov eax, 4
    mov ebx, 1
    mov ecx, str_msg2
    mov edx, str_msg2_len
    int 0x80
    mov eax, 1
    int 0x80

self_write:
    push ebp
    mov ebp, esp
    mov eax, 0x4
    mov ebx, [ebp + 8]
    mov ecx, [ebp + 12]
    mov edx, [ebp + 16]
    int 0x80
    mov eax, 1
    pop ebp
    ret