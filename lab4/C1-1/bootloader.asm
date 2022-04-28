[bits 16]

        mov     ax,0xb800
        mov     es,ax
        mov     ax,0x07e0
        mov     ds,ax
        mov     si,string

        mov     cx,21
        mov     di,1600
        mov     ah,3
label2: lodsb
        stosw
        loop    label2

here2:  jmp     here2
    
    	string db   "Running bootloader..."
    	times 2560-($-$$) db 0
