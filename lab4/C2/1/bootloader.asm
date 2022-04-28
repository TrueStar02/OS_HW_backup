[bits 16]

        mov     ax,0xb800
        mov     es,ax
        mov     ax,0x07e0
        mov     ds,ax
        mov     si,string1

        mov     cx,21
        mov     di,1600
        mov     ah,3
label2: lodsb
        stosw
        loop    label2

        mov     dword[0x8800],0
        mov     dword[0x8804],0
        mov     dword[0x8808],0x0000ffff
        mov     dword[0x880c],0x00cf9200
        mov     dword[0x8810],0x00000000
        mov     dword[0x8814],0x00409600
        mov     dword[0x8818],0x80007fff
        mov     dword[0x881c],0x0040920b
        mov     dword[0x8820],0x0000ffff
        mov     dword[0x8824],0x00cf9800

        mov     word[pgdt],39
        lgdt    [pgdt]

        in      al,0x92
        or      al,2
        out     0x92,al

        cli
        mov     eax,cr0
        or      eax,1
        mov     cr0,eax

        jmp     dword 0x0020:protect_mode_tag

[bits 32]

protect_mode_tag:
        mov     ax,0x8
        mov     ds,ax
        mov     es,ax
        mov     ax,0x10
        mov     ss,ax
        mov     ax,0x18
        mov     gs,ax

        mov     ah,6
        mov     ebx,1920
        mov     ecx,26
        mov     esi,string2

label3: lodsb
        mov     [gs:ebx],ax
        inc     ebx
        inc     ebx
        loop    label3

	      jmp     $
    
    	string1 db      "Running bootloader..."
        string2 db      "Running in protect mode..."

        pgdt    dw      0
                dd      0x00008800

    	times 2560-($-$$) db 0
