[bits 32]

global  asm_hello_world
global  asm_lidt
global  default_interrupt_handler

asm_hello_world:
        push	ebp
        mov		ebp,esp
        push    eax
        push    ebx
        push    ecx
        mov     ah,0x0b
        mov     ebx,2240
        mov     ecx,21
        mov     esi,string3

rp1:    lodsb
        mov     [gs:ebx],ax
        inc     ebx
        inc     ebx
        loop    rp1

        mov     ebx,2400
        mov     ecx,19
        mov     esi,string4

rp2:    lodsb
        mov     [gs:ebx],ax
        inc     ebx
        inc     ebx
        loop    rp2

        pop     ecx
        pop     ebx
        pop     eax
        pop		ebp
        ret

asm_lidt:
        push    ebp
        mov     ebp,esp
        push    eax
        mov     eax,[ebp+4*3]
        mov     [idt_buf],ax
        mov     eax,[ebp+4*2]
        mov     [idt_buf+2],eax
        lidt    [idt_buf]
        pop     eax
        pop 	ebp
        ret

default_interrupt_handler:
        cli
        mov     esi,err_msg
        mov     ebx,2720
        mov     ah,12
        mov     ecx,33
rp3:    lodsb
        mov     [gs:ebx],ax
        inc     ebx
        inc     ebx
        loop    rp3
        jmp     $
        ret

        string3 db      "Hello world in kernel"
        string4 db      "20337171 BianRunhua"

        err_msg db      "Default interrupt handler,halt..."

        idt_buf dw      0
                dd      0


