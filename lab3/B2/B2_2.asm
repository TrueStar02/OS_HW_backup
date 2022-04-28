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

        mov     bx,0
        mov     ah,2
        mov     dl,12
        mov     dh,12
        int     0x10

        mov     cx,20
        mov     si,string
        mov     bx,color
label:  lodsb
        mov     dh,[bx]
        inc     bx
        push    bx
        push    cx
        mov     ah,9
        mov     cx,1
        mov     bh,0
        mov     bl,dh
        int     0x10
        inc     dl
        mov     dh,12
        mov     ah,2
        mov     bh,0
        int     0x10
        pop     cx
        pop     bx
        loop    label
        
		string	db		"20337171 BianRunhua"
        color   db      0x08,0x09,0x0a,0x0b,0x0c
        		db		0x0d,0x0e,0x0f,0x00,0x70
                db      0x71,0x72,0x73,0x74,0x75
                db		0x76,0x70,0x71,0x72,0x00
       
here:   jmp     here

times 	510-($-$$) db 0
	dw	0xaa55

