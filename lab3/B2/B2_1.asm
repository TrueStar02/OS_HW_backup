[bits 16]

        mov     bx,0
        mov     ah,3
        int     0x10
        add     dh,3
        add     dl,5
        mov     ah,2
        int     0x10
       
here:   jmp     here

times 	510-($-$$) db 0
	dw	0xaa55

