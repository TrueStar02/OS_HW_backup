		org		0x7c00

[bits 16]

        xor     ax,ax
        mov     fs,ax
        mov     gs,ax
        mov     ds,ax
        mov     es,ax

        mov     sp,0x7c00
        mov     bx,0x7e00

        mov     ax,5
        push    ax
        mov     ax,1
        push    ax
        push    bx
        call    read_disk
        add     sp,6
        mov     ax,0xe801
        int     0x15
        mov     [0x7df0],ax
        mov     [0x7df2],bx
        
        jmp	0x0000:0x7e00
here:	jmp 	here

read_disk:
        push    bp
        mov     bp,sp
        push    ax
        push    di
        push    cx
        push    dx

        mov     ax,[bp+2*3]
        mov     dx,0x1f3
        out     dx,ax

        inc     dx
        inc     dx
        xor     ax,ax
        or      ax,0xe000
        out     dx,ax

        mov     dx,0x1f2
        mov     ax,[bp+2*4]
        mov     cx,ax
        out     dx,al
        
        mov		dx,0x1f7
        mov		al,0x20
        out		dx,al
        mov     di,[bp+2*2]

rd:     push    cx
		mov		dx,0x1f7
label:  in      al,dx
        and     al,0x89
        cmp     al,0x08
        jnz     label
        mov     cx,256
        mov     dx,0x1f0  
rep     insw
        pop     cx
        loop    rd

        pop     dx
        pop     cx
        pop     di
        pop     ax
        pop     bp
        ret

        times   510 - ($ - $$) db 0
        dw      0xaa55
