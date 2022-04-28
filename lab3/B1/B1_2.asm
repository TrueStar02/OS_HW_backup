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
        mov     ax,0xb800
        mov     es,ax

        mov     cx,20
        mov     si,string
        mov     di,1944
        mov     bx,color
label:  lodsb
        mov     ah,[bx]
        inc     bx
        stosw
        loop	label

		string	db		"20337171 BianRunhua"
        color   db      0x08,0x09,0x0a,0x0b,0x0c
        		db		0x0d,0x0e,0x0f,0x00,0x70
                db      0x71,0x72,0x73,0x74,0x75
                db		0x76,0x70,0x71,0x72,0x00
here:   jmp     here

times 	510-($-$$) db 0
	dw	0xaa55

