[bits 16]
        mov     ah,0x0e
        mov     cx,0x100
repeat: mov     al,0x0d
        int     0x10
        mov     al,0x0a
        int     0x10
        loop    repeat

        mov     dh,0x0c
        mov     dl,0x20
        mov     ah,0x02
        int     0x10
    
here:   jmp     here

times   510-($-$$)  db  0
        dw      0xaa55
