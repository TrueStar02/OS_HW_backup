;		org		0x7c00

[bits 16]

        xor     ax,ax
        mov     fs,ax
        mov     gs,ax
        mov     ds,ax
        mov     es,ax

        mov     sp,0x7c00
        mov     bx,0x7e00

        mov     ax,0x0205
        mov     cx,0x0002
        mov     dx,0x0080
        int     0x13
        
        jmp		0x0000:0x7e00
here:	jmp 	here


        times   510 - ($ - $$) db 0
        dw      0xaa55
