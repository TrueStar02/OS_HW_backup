[bits 16]

        mov     ax,0x07c0
        mov     ds,ax
        mov     ax,0xb800
        mov     es,ax

        mov		ah,0x01
        mov     cx,11
        mov     si,string
        mov     di,0
label:  lodsb
        stosw
        loop	label

		string	db	"Hello world"

here:   jmp     here

times 	510-($-$$) db 0
		dw		0xaa55

