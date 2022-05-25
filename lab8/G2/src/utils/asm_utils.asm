[bits 32]

global  asm_hello_world
global  asm_lidt
global  default_interrupt_handler
global  asm_time_interrupt_handler
global  asm_switch_thread
global  asm_atomic_exchange
global  asm_lock
global  asm_init_page_reg

extern c_time_interrupt_handler

asm_hello_world:
        push	ebp
        mov	ebp,esp
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

asm_time_interrupt_handler:
        pusha
    
        mov al, 0x20
        out 0x20, al
        out 0xa0, al
        
        call c_time_interrupt_handler

        popa
        iret

asm_switch_thread:
        push    ebp
        push    eax
        push    ebx
        push    edi
        push    esi     

        mov     eax, [esp + 6 * 4]
        mov     [eax], esp 

        mov     eax, [esp + 7 * 4]
        mov     esp, [eax] 

        pop     esi
        pop     edi
        pop     ebx
        pop     eax
        pop     ebp

        sti
        ret

asm_atomic_exchange:
        push    ebp
        mov     ebp, esp
        pusha

        mov     ebx,[ebp+4*2] 
        mov     eax,[ebx]         
        mov     ebx,[ebp+4*3] 
        xchg    [ebx],eax        
        mov     ebx,[ebp+4*2] 
        mov     [ebx],eax          

        popa
        pop     ebp
        ret

asm_lock:
        push    ebp
        mov     ebp,esp
        push    ebx

        mov     ebx,[ebp+4*2]
rp4:    lock    bts     dword[ebx],0
        jc      rp4

        pop     ebx
        pop     ebp
        ret
        
asm_init_page_reg:
        push    ebp
        mov     ebp, esp

        push    eax

        mov     eax, [ebp + 4 * 2]
        mov     cr3, eax 
        mov     eax, cr0
        or      eax, 0x80000000
        mov     cr0, eax           

        pop     eax
        pop     ebp

        ret

        string3 db      "Hello world in kernel"
        string4 db      "20337171 BianRunhua"

        err_msg db      "Default interrupt handler,halt..."

        idt_buf dw      0
                dd      0


