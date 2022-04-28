[bits 32]

global  asm_hello_world

asm_hello_world:
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
        ret

        string3 db      "Hello world in kernel"
        string4 db      "20337171 BianRunhua"

