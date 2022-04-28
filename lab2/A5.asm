[bits 16]

        mov     ah,0x0e
        mov     cx,0x100
repeat: mov     al,0x0d
        int     0x10
        mov     al,0x0a
        int     0x10
        loop    repeat

        push    ds
        mov     ax,0x07c0
        mov     ds,ax
        push    es
        mov     ax,0xb800
        mov     es,ax

        mov     di,1828
        mov     si,string1
        mov     cx,11
        
rep1:   lodsb
        stosb
        mov     al,201
        stosb
        loop    rep1

        mov     di,1992
        mov     si,string2
        mov     cx,8

        mov     di,1992
        mov     si,string2
        mov     cx,8

rep2:   lodsb
        stosb
        mov     al,82
        stosb
        loop    rep2

        mov     di,2148
        mov     si,string3
        mov     cx,12

rep3:   lodsb
        stosb
        mov     al,39
        stosb
        loop    rep3

        pop     ax
        mov     es,ax
        pop     ax
        mov	ds,ax

        string1 db      "Bian Runhua"
        string2 db      "20337171"
        string3 db      "Hello,world!"        
    
here:   jmp     here
times   510-($-$$)  db  0
        dw      0xaa55
    


