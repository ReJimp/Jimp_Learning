%include "boot.inc"
section loader vstart=LOADER_BASE_ADDR
LOADER_STACK_TOP equ LOADER_BASE_ADDR

; 构建GDT及其内部的描述符
GDT_BASE:           dd 0x00000000
                    dd 0x00000000

CODE_DESC:          dd 0x0000FFFF
                    dd DESC_CODE_HIGH4

DATA_STACK_DESC:    dd 0x0000FFFF
                    dd DESC_DATA_HIGH4

VIDEO_DESC:         dd 0x80000007                   ; limit=(0xbffff-0xb8000) / 4K = 0x7
                    dd DESC_VIDEO_HIGH4             ; dpl=0

GDT_SIZE    equ     $ - GDT_BASE
GDT_LIMIT   equ     GDT_SIZE - 1
times       60      dq         0                    ; 预留60个描述符


total_mem_bytes     dd         0                    ; 位于内存0xb00(0x900+0x200)处，记录内存容量

; 相当于(CODE_DESC - GDT_BASE) / 8 + TI_GDT + RPL0        
SELECTOR_CODE   equ     (0x0001 << 3) + TI_GDT + RPL0   
SELECTOR_DATA   equ     (0x0002 << 3) + TI_GDT + RPL0
SELECTOR_VIDEO  equ     (0x0003 << 3) + TI_GDT + RPL0

; 以下时gdt的指针,前两字节是gdt界限,后4字节是gdt的起始地址
gdt_ptr     dw      GDT_LIMIT
            dd      GDT_BASE

; 对齐到256字节：total_mem_bytes + gdt_ptr + ards_buf + ards_nr = 4 + 6 + 244 + 2 = 256 = 0x100
ards_buf times 244 db 0
ards_nr  dw    0                                    ; 记录ards结构体数量

loader_start:
; 获取内存布局方法1
; ------------------------------------------------
; int 0x15;eax=0000E820h;edx=534D4150h('SMAP')
; ------------------------------------------------
    xor ebx, ebx                                    ; 首次调用需将ebx置0
    mov edx, 0x534D4150                             ; edx赋值为'SMAP'，后续不改变
    mov di, ards_buf                                ; ards结构缓冲区
.e820_mem_get_loop:                                 ; 循环获取调用返回的ards结构体
    mov eax, 0x0000E820                             ; 子调用号，调用后eax的值变为0x534d4150
    mov ecx, 20                                     ; ards结构体大小
    int 0x15
    jc .e820_failed_so_try_e801                     ; cf为1说明发生错误，改为调用0xe801
    
    add di, cx                                      ; ards缓冲区指针偏移20字节
    inc word [ards_nr]                              ; ards结构体数量ards_nr增1
    cmp ebx, 0                                      ; 此时ebx为0时说明已返回所有ards
    jnz .e820_mem_get_loop

; 遍历所有ards，找到最大的(base_add_low + length_low)，即可得到内存的容量
    mov cx, [ards_nr]
    mov ebx, ards_buf
    xor edx, edx
.find_max_mem_area:
    mov eax, [ebx]                                  ; BaseAddrLow
    add eax, [ebx + 8]                              ; BaseAddrHigh
    add ebx, 20                                     ; 指针指向下一个ards结构
    cmp edx, eax
    jge .next_ards
    mov edx, eax

.next_ards:
    loop .find_max_mem_area
    jmp .mem_get_ok

; 获取内存布局方法2
; ------------------------------------------------
; int 0x15;ax=E801h;
; 返回值ax与cx同记录低16MB(单位KB)
; 返回值bx与dx同记录16MB~4GB(单位64KB)
; ------------------------------------------------
.e820_failed_so_try_e801:
    mov ax, 0xE801
    int 0x15
    jc .e801_failed_so_try88                        ; cf为1说明发生错误，改为调用0x88
    ; 计算低16MB
    mov cx, 0x400                                   ; 换算成字节
    mul cx
    shl edx, 16                                     ; 计算结果高位在dx,低位在ax
    and eax, 0x0000FFFF
    or edx, eax
    add edx, 0x100000                               ; 加上1MB
    mov esi, edx
    ; 计算16MB~4GB
    xor eax, eax
    mov ax, bx
    mov ecx, 0x10000
    mul ecx
    add esi, eax                                    ; 高位edx为0
    mov edx, esi
    jmp .mem_get_ok

; 获取内存布局方法3
; ------------------------------------------------
; int 0x15;ah=88h;只能获取64MB以内的
; ------------------------------------------------
.e801_failed_so_try88:
    mov ah, 0x88
    int 0x15
    jc .error_hlt
    and eax, 0x0000FFFF

    mov cx, 0x400
    mul cx
    shl edx, 16
    or edx, eax
    add edx, 0x100000

.mem_get_ok:
    mov [total_mem_bytes], edx                      ; 存储内存容量  

; ------------------------------------------------
;               准备进入保护模式
; ------------------------------------------------
; 1 打开A20
; 2 加载GDT
; 3 CR0的PE位置1
; ------------------------------------------------

; ------------------打开A20------------------------
    in al, 0x92
    or al, 0000_0010b
    out 0x92, al

; ------------------加载GDT------------------------
    lgdt [gdt_ptr]

; -----------------cr0第0位置1---------------------
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax
    jmp dword SELECTOR_CODE:p_mode_start            ; 刷新流水线，地址依然对应物理地址

.error_hlt:
    hlt

[bits 32]
p_mode_start:
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, LOADER_STACK_TOP
    mov ax, SELECTOR_VIDEO
    mov gs, ax

; ------------------加载内核------------------------    
    mov eax, KERNEL_START_SECTOR                    ; index为9的扇区
    mov ebx, KERNEL_BIN_BASE_ADDR
    mov ecx, 200                                    ; 设置200个扇区

    call rd_disk_m_32                               ; 拷贝磁盘的kernel.bin到内存

    ; 创建页目录及页表并初始化页内存位图
    call setup_page
    ; 将描述符表地址及偏移量写入内存gdt_ptr
    sgdt [gdt_ptr]    
    ; 获取GDT及地址
    mov ebx, [gdt_ptr + 2]
    ; 修改显存的基地址
    or dword [ebx + 0x18 + 4], 0xc0000000
    ; 修改GDT的基地址
    add dword [gdt_ptr + 2], 0xc0000000
    ; 修改栈指针地址
    add esp, 0xc0000000    
    ; 把页目录地址给cr3
    mov eax, PAGE_DIR_TABLE_POS
    mov cr3, eax
    ; 开启cr0的pg位(第31位)
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ; 重新加载gdt
    lgdt [gdt_ptr]

    ; 更新流水线
    jmp SELECTOR_CODE:enter_kernel

enter_kernel:
    call kernel_init
    mov esp, 0xC009F000
    jmp KERNEL_ENTRY_POINT

; --------拷贝kernel.bin中的segment到编译的地址-------
kernel_init:
    xor eax, eax
    xor ebx, ebx                                    ; 记录程序头表地址
    xor ecx, ecx                                    ; 记录程序头表条目数量
    xor edx, edx                                    ; 记录程序头表条目大小

    mov dx, [KERNEL_BIN_BASE_ADDR + 42]             ; e_phentsize
    mov ebx, [KERNEL_BIN_BASE_ADDR + 28]            ; e_phoff
    add ebx, KERNEL_BIN_BASE_ADDR                   ; 程序头表基地址
    mov cx, [KERNEL_BIN_BASE_ADDR + 44]             ; e_phnum

.each_segment:
    cmp byte [ebx + 0], PT_NULL
    ; 若type=PT_NULL，说明此条目未使用
    je .PTNULL

    push dword [ebx + 16]                           ; memcpy第三个参数，p_filesz
    mov eax, [ebx + 4]                              ; p_offset
    add eax, KERNEL_BIN_BASE_ADDR                   ; memcpy第二个参数，段的物理地址
    push eax
    push dword [ebx + 8]                            ; memcpy第一个参数，目标地址p_vaddr
    call mem_cpy
    add esp, 12                                     ; 平衡堆栈

.PTNULL:
    add ebx, edx
    loop .each_segment

    ret

; -----------------mem_cpy实现---------------------
;   参数列表: dest, src, size
; ------------------------------------------------
mem_cpy:
    cld
    push ebp
    mov ebp, esp
    push ecx

    mov edi, [ebp + 8]
    mov esi, [ebp + 12]
    mov ecx, [ebp + 16]
    rep movsb

    ; 恢复现场
    pop ecx
    pop ebp
    ret

; ---------------创建页目录以及页表-------------------
setup_page:
; 页目录初始化为0
    mov ecx, 0x1000                                 ; 页目录大小0x1000                 
    mov esi, 0
.clear_page_dir:
    mov byte [PAGE_DIR_TABLE_POS + esi], 0
    inc esi
    loop .clear_page_dir

; 创建第一个页目录项(PDE)
.create_pde:  
    mov eax, PAGE_DIR_TABLE_POS          
    add eax, 0x1000
    mov ebx, eax

    or eax, PG_US_U | PG_RW_W | PG_P                ; 属性为7,US=1,RW=1,P=1   
    mov [PAGE_DIR_TABLE_POS + 0x0], eax
    mov [PAGE_DIR_TABLE_POS + 0xc00], eax           ; 第0和第768个页目录项都指向第一个页表，0~767所指为低3GB空间，768~1022所指为高1GB空间, 内核空间指向低4MB内存
    sub eax, 0x1000
    mov [PAGE_DIR_TABLE_POS + 0x1000 - 0x4], eax    ; 最后一个页目录项指向页目录基地址

; 创建页表
    mov ecx, 256                                    ; 1M 低端内存 / 每页大小 4k = 256，内核就用到低1MB
    mov esi, 0
    mov edx, PG_US_U | PG_RW_W | PG_P
.create_pte:
    mov [ebx + esi * 4], edx
    add edx, 0x1000                                 ; 页表每项指向4KB
    inc esi
    loop .create_pte

; 创建内核页表的页目录项, 填充满，防止出现内核不共享
    mov eax, PAGE_DIR_TABLE_POS
    add eax, 0x2000                                 ; 第二个页表
    or eax, PG_US_U | PG_RW_W | PG_P
    mov ebx, PAGE_DIR_TABLE_POS
    mov ecx, 254                                    ; 范围为第769~1022的所有目录项数量
    mov esi, 769                                    ; 内核区域的起始页目录
.create_kernel_pde:
    mov [ebx + esi * 4], eax
    inc esi
    add eax, 0x1000
    loop .create_kernel_pde

    ret

; ----------------读取n个磁盘扇区--------------------
rd_disk_m_32:
    mov esi, eax
    mov di, cx

    mov dx, 0x1f2
    mov al, cl
    out dx, al
    mov eax, esi

    mov dx, 0x1f3
    out dx, al

    mov cl, 8
    shr eax, cl
    mov dx, 0x1f4
    out dx, al

    shr eax, cl
    mov dx, 0x1f5
    out dx, al

    shr eax, cl
    and al, 0x0f
    or al, 0xe0
    mov dx, 0x1f6
    out dx, al

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

.not_ready:
    nop
    in al, dx
    and al, 0x88
    cmp al, 0x08
    jnz .not_ready

    mov ax, di
    mov dx, 256
    mul dx
    mov cx, ax
    mov dx, 0x1f0

.go_on_read:
    in ax, dx
    mov [ebx], ax
    add ebx, 2
    loop .go_on_read

    ret