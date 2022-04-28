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
        mov     cx,25

        lea     si,[string]
        mov     dx,28
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

        mov     si,colors
        mov     bh,0
        mov     al,0x30
        mov     di,0

here:   cmp     byte [si],0
        jnz     label2
        mov     si,colors
label2: mov     bl,[si]
        inc     si
        mov     ah,9
        mov     cx,1
        int     0x10
        inc     al
        mov     cx,0x194f
        sub     cx,dx
        mov     dx,cx
        mov     ah,2
        int     0x10
        mov     ah,9
        mov     cx,1
        int     0x10    
        inc     al
        cmp     al,0x3a
        jl      label3
        mov     al,0x30
label3: mov     cx,0x194f
        sub     cx,dx
        mov     dx,cx
        test    di,0x1
        jnz     csub
        inc     dl
        jmp     cend
csub:   dec     dl
cend:   test    di,0x2
        jnz     rsub
        inc     dh
        jmp     rend
rsub:   dec     dh
rend:   cmp     dh,1
        jz      rrev
        cmp     dh,24
        jz      rrev
rfin:   cmp     dl,0
        jz      crev
        cmp     dl,79
        jz      crev
cfin:   mov     ah,2
        int     0x10
        push    dx
        mov     ah,0x86
        mov     cx,6
        mov     dx,0
        int     0x15
        pop     dx
        jmp     here

rrev:   xor     di,0x2
        jmp     rfin
crev:   xor     di,0x1
        jmp     cfin

        string  db      " BRH's character catapult"
        colors  db      0x09,0x0a,0x0c,0x0e,0x0f
                db      0x18,0x1c,0x1d,0x1e,0x1f
                db      0x58,0x59,0x5a,0x5e,0x5f 
                db      0x70,0x71,0x72,0x73,0x74
                db      0x30,0x3c,0x3d,0x3e,0x3f,0x00

times 	510-($-$$) db 0
	dw	0xaa55

