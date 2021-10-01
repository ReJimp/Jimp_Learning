TI_GDT          equ         0
RPL0            equ         0
SELECTOR_VIDEO  equ         (0x0003 << 3) + TI_GDT + RPL0

section .data
put_int_buffer  dq          0           ; 用于整数到字符串的转换

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
    mov ecx, 960                        ; (2000-80) * 2 / 4 = 960
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

;---------------------put_str----------------------
; 功能:通过 put_char 来打印以 0 字符结尾的字符串
;--------------------------------------------------
global put_str
put_str:
    ; 函数只需备份EBX与ECX
    push ebx
    push ecx
    xor ecx, ecx
    mov ebx, [esp + 12]                 ;获取字符串地址

.goon:
    mov cl, [ebx]
    cmp cl, 0
    jz .str_over
    push ecx
    call put_char
    add esp, 4
    inc ebx
    jmp .goon

.str_over:
    pop ecx
    pop ebx
    ret

;---------------------put_int----------------------
; 功能:通过 put_char 来打印整数
;--------------------------------------------------
global put_int
put_int:
    pushad
    mov ebp, esp
    mov eax, [ebp + 4 * 9]
    mov edx, eax
    mov edi, 7                          ; 采用大端序，此时为put_int_buffer最后一字节偏移量
    mov ecx, 8                          ; 比较八次，4位二进制数字可转化为1位十六进制数字，一共32位二进制位
    mov ebx, put_int_buffer

.16based_4bits:
    and edx, 0x0000000F                 ; 获取低4位
    cmp edx, 9
    jg .is_A2F
    add edx, '0'
    jmp .store

.is_A2F:
    sub edx, 10
    add edx, 'A'

.store:
    mov [ebx + edi], dl                 ; 存储一字节
    dec edi
    shr eax, 4
    mov edx, eax
    loop .16based_4bits

.ready_to_print:
    inc edi                             ; 循环结束时其为-1，将EDI恢复到0

.skip_prefix_0:
    cmp edi, 8                          ; 表示全为0
    je .full0

.go_on_skip:
    mov cl, [put_int_buffer + edi]
    inc edi
    cmp cl, '0'
    je .skip_prefix_0
    dec edi                             ; 不为0则回退到当前数字
    jmp .put_each_num

.full0:
    mov cl, '0'                         ; 只需打印'0'

.put_each_num:
    push ecx
    call put_char
    add esp, 4
    inc edi
    mov cl, [put_int_buffer + edi]
    cmp edi, 8
    jl .put_each_num
    popad
    ret