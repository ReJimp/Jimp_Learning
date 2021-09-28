TI_GDT          equ         0
RPL0            equ         0
SELECTOR_VIDEO  equ         (0x0003 << 3) + TI_GDT + RPL0

[bits 32]
section .text
;---------------------put_char---------------------
; 功能:将栈中的1个字符写到光标所在处
;--------------------------------------------------
global put_char
put_char:
    pushad                              ; 保存寄存器
    mov ax, SELECTOR_VIDEO              ; 获取选择子
    mov gs, ax

    ; 获取当前光标位置高8位
    mov dx, 0x03d4                      ; 索引寄存器
    mov al, 0x0e                        ; 提供光标位置高8位
    out dx, al
    mov dx, 0x03d5
    in al, dx
    mov ah, al                          ; 获取高8位

    ; 获取当前光标位置低8位
    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    in al, dx                           ; 获取低8位

    mov bx, ax                          ; bx默认基址
    mov ecx, [esp + 36]                 ; 获取待打印字符，pushad：32字节，ret：4字节

    cmp cl, 0xd                         ; 换行=0xd
    jz .is_carriage_return
    cmp cl, 0xa                         ; 回车=0xa
    jz .is_line_feed
    cmp cl, 0x8                         ; backspace
    jz .is_backspace
    jmp .put_other

.is_backspace:
    dec bx
    shl bx, 1                           ; 显示1个字符需要2字节，光标号*2=显存偏移
    
    mov byte [gs:bx], 0x20
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    jmp .set_cursor

.put_other:
    shl bx, 1
    mov [gs:bx], cl
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    inc bx
    cmp bx, 2000
    jl .set_cursor

.is_line_feed:
.is_carriage_return:
    xor dx, dx                          ; dx为被除数高16位
    mov ax, bx                          ; ax为被除数低16位
    mov si, 80
    div si
    sub bx, dx                          ; 光标值减去余数取整

.is_carriage_return_end:
    add bx, 80
    cmp bx, 2000

.is_line_feed_end:
    jl .set_cursor

;第0屏行范围是0～24，将屏幕的第1～24行搬运到第0～23行，清空第24行，可实现滚屏
.roll_screen:
    cld                                 ; DF标志位清0
    mov ecx, 960                        ; (2000-80)*2/4=960
    mov esi, 0xc00b80a0                 ; 第一行行首
    mov edi, 0xc00b8000                 ; 第零行行首
    rep movsd

    ; 最后一行填充为空白
    mov ebx, 3840                       ; 1920*2
    mov cx, 80

.cls:
    mov word [gs:ebx], 0x0720
    add ebx, 2
    loop .cls
    mov bx, 1920                        ; 光标重置为最后一行行首

; 将光标设置为bx值
.set_cursor:
    ; 设置高8位
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    mov al, bh
    out dx, al

    ; 设置低8位
    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    mov al, bl
    out dx, al

.put_char_done:
    popad
    ret