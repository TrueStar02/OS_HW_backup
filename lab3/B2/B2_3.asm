[bits 16]

        mov     ah,0x0e			
        mov     cx,0x100		
repeat: mov     al,0x0d
        int     0x10
        mov     al,0x0a
        int     0x10
        loop    repeat

        mov     ax,0x07c0
        mov     ds,ax
        mov     cx,18

        lea     si,[string]
        mov     dx,31
label:  lodsb
        push    cx
        mov     ah,0x09
        mov     cx,1
        mov     bx,0x0071
        int     0x10
        inc     dl
        mov     ah,2
        mov     bh,0
        int     0x10
        pop     cx
        loop    label

        mov     bx,0x000f
        mov     cx,1
        mov     ah,2
        mov     dx,0x0100
        int     0x10

here:   mov     ah,0
        int     0x16    
        cmp     al,8
        jz      back        
        inc     dl
        cmp     dl,80
        jnz     ctn
        cmp     dh,24
        jz      fence
        inc     dh
        xor     dl,dl
ctn:    mov     ah,0x09
        int     0x10
        mov     ah,0x02
        int     0x10
        jmp     here

fence:  dec     dl
        jmp     here

back:   cmp     dl,0
        jnz     ctn2
        cmp     dh,1
        jz      here
        add     dl,80
        dec     dh
ctn2:   dec     dl
        mov     ah,0x02
        int     0x10
        mov     ax,0x0920
        int     0x10
        jmp     here

        string  db      " BRH's text editor"

times 	510-($-$$) db 0
	dw	0xaa55

