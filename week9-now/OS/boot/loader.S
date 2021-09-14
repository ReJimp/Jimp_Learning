%include "boot.inc"
section loader vstart=LOADER_BASE_ADDR
LOADER_STACK_TOP equ LOADER_BASE_ADDR
jmp loader_start

;构建GDT及其内部的描述符
GDT_BASE:           dd 0x00000000
                    dd 0x00000000

CODE_DESC:          dd 0x0000FFFF
                    dd DESC_CODE_HIGH4

DATA_STACK_DESC:    dd 0x0000FFFF
                    dd DESC_DATA_HIGH4

VIDEO_DESC:         dd 0x80000007       ; limit=(0xbffff-0xb8000)/4K=0x7
                    dd DESC_VIDEO_HIGH4 ; dpl=0

GDT_SIZE    equ     $ - GDT_BASE
GDT_LIMIT   equ     GDT_SIZE - 1
times       60      dq         0        ;预留60个描述符

SELECTOR_CODE   equ     (0x0001 << 3) + TI_GDT + RPL0        
SELECTOR_DATA   equ     (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VIDEO  equ     (0x0003 << 3) + TI_GDT + RPL0


;以下时gdt的指针,前两字节是gdt界限,后4字节是gdt的起始地址
gdt_ptr             dw GDT_LIMIT
                    dd GDT_BASE

loadermsg           db 'loader run in real'


loader_start:
; 清屏利用0x06号功能，上卷全部行，则可清屏
; ------------------------------------------------
;INT 0x10   功能号:0x06   功能描述:上卷窗口
;-------------------------------------------------
;输入:
;AH 功能号 = 0x06
;AL = 上卷的行数(如果为0,表示全部)
;BH = 上卷行属性
;(CL,CH) = 窗口左下角的(X,Y)位置
;(DL,DH) = 窗口右下角的(X,Y)位置
;无返回值:
    mov ax, 0x600
    mov bx, 0x700
    mov cx, 0                       ; 左上角: (0,0)
    mov dx, 0x184f                  ; 右下角: (80,25)
                                    ; VGA文本模式中,一行只能容纳80个字符,共25行
                                    ; 下标从0开始,所以0x18=24，0x4f=79
    int 0x10                        ; int 0x10


; ------------------------------------------------
;INT 0x10   功能号:0x13   功能描述:打印字符串
;-------------------------------------------------
;输入: 
;AH 子功能号=13H 
;BH = 页码 
;BL = 属性(若 AL=00H 或 01H) 
;CX=字符串长度 
;(DH､ DL)=坐标(行､列) 
;ES:BP=字符串地址 
;AL=显示输出方式 
;  0—字符串中只含显示字符,其显示属性在BL中,显示后,光标位置不变 
;  1—字符串中只含显示字符,其显示属性在BL中,显示后,光标位置改变 
;  2—字符串中含显示字符和显示属性.显示后,光标位置不变 
;  3—字符串中含显示字符和显示属性.显示后,光标位置改变 
;无返回值
    mov sp, LOADER_BASE_ADDR
    mov bp, loadermsg               ; ES:BP = 字符串地址
    mov cx, 0x12                    ; CX = 字符串长度
    mov ax, 0x1301                  ; AH = 13,  AL = 01h          
    mov bx, 0x001f                  ; 页号为 0(BH = 0) 蓝底粉红字(BL = 1fh)
    mov dx, 0x1800
    int 0x10                        ; 10h 号中断


; ------------------------------------------------
;               准备进入保护模式
; ------------------------------------------------
; 1 打开A20
; 2 加载GDT
; 3 CR0的PE位置1
; ------------------------------------------------

; ------------------打开A20-----------------------
    in al, 0x92
    or al, 0000_0010b
    out 0x92, al

; ------------------加载GDT----------------------
    lgdt [gdt_ptr]

; -----------------cr0第0位置1-------------------
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax

    jmp dword SELECTOR_CODE:p_mode_start    ;刷新流水线

[bits 32]
p_mode_start:
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, LOADER_STACK_TOP
    mov ax, SELECTOR_VIDEO
    mov gs, ax

    mov byte [gs:0x00], 'P'
    mov byte [gs:0x01], 0xA4

    jmp $