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
        mov     di,0x0200

        mov     dx,0x1f3
        out     dx,ax
        inc     dx
        inc     dx
        and		ax,0x0f00
        or      ah,0xe0
        out     dx,ax
        mov     dx,0x1f2
        mov     al,5
        out     dx,al
        mov     dx,0x01f7
        mov     al,0x20
        out     dx,al

check:  in      al,dx
        and     al,0x89
        xor     al,0x08
        jnz     check

        mov     cx,1280
        mov     dx,0x1f0
label:  in      ax,dx
        stosw
        loop    label

        jmp     0x07e0:0x0000

        times   510 - ($ - $$) db 0
        dw      0xaa55
