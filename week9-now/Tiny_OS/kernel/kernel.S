[bits 32]
%define ERROR_CODE nop                  ; 若CPU自动压入错误码，则无需进行操作
%define ZERO push 0                     ; 若CPU未压入错误码，则需手动占用错误码的栈空间
                                        ; 错误码需要手动弹出

extern idt_table                        ; 中断处理函数地址数组

section .data
global intr_entry_table
intr_entry_table:

%macro VECTOR 2                         ; 宏的参数个数为2
section .text                           ; 用向量号标识对应的中断处理程序
intr%1entry:
    %2
    ; 保留上下文环境
    push ds
    push es
    push fs
    push gs
    pushad

    mov al, 0x20
    out 0xa0, al
    out 0x20, al

    push %1

    call [idt_table + %1 * 4]
    add esp, 4
    jmp intr_exit

section .data
    dd intr%1entry
%endmacro

section .text
global intr_exit
intr_exit:
    ; 恢复上下文
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 4                          ; 跳过错误码
    iretd

VECTOR 0x00, ZERO
VECTOR 0x01, ZERO
VECTOR 0x02, ZERO
VECTOR 0x03, ZERO 
VECTOR 0x04, ZERO
VECTOR 0x05, ZERO
VECTOR 0x06, ZERO
VECTOR 0x07, ZERO 
VECTOR 0x08, ERROR_CODE
VECTOR 0x09, ZERO
VECTOR 0x0a, ERROR_CODE
VECTOR 0x0b, ERROR_CODE 
VECTOR 0x0c, ZERO
VECTOR 0x0d, ERROR_CODE
VECTOR 0x0e, ERROR_CODE
VECTOR 0x0f, ZERO 
VECTOR 0x10, ZERO
VECTOR 0x11, ERROR_CODE
VECTOR 0x12, ZERO
VECTOR 0x13, ZERO 
VECTOR 0x14, ZERO
VECTOR 0x15, ZERO
VECTOR 0x16, ZERO
VECTOR 0x17, ZERO 
VECTOR 0x18, ERROR_CODE
VECTOR 0x19, ZERO
VECTOR 0x1a, ERROR_CODE
VECTOR 0x1b, ERROR_CODE 
VECTOR 0x1c, ZERO
VECTOR 0x1d, ERROR_CODE
VECTOR 0x1e, ERROR_CODE
VECTOR 0x1f, ZERO 
VECTOR 0x20, ZERO