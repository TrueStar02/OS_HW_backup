[bits 16]

        mov     ax,0x07c0
        mov     ds,ax
        mov     es,ax
        xor     ax,ax
        mov     fs,ax
        mov     gs,ax

        mov     sp,0x7c00
        mov     ax,1
        mov     cx,0
        mov     bx,0x0200

        mov     ax,0x0205
        mov     cx,0x0002
        mov     dx,0x0080
        int     0x13

        jmp     0x07e0:0x0000

here:   jmp     here

        times   510 - ($ - $$) db 0
        dw      0xaa55
