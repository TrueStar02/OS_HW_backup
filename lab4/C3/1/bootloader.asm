;		org		0x7e00
		
[bits 16]
	    
        mov     ax,0xb800
        mov     es,ax
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
        
        mov	cx,1000
        mov	ax,0
 	mov	bx,0
label3:	mov	[gs:ebx],ax
	inc	ebx
	inc	ebx
	loop	label3

        mov     ah,0x71
        mov     ebx,52
        mov     ecx,28
        mov     esi,string2
        

label4: lodsb
        mov     [gs:ebx],ax
        inc     ebx
        inc     ebx
        loop    label4

        mov     dx,0x0100
        mov     esi,colors
        mov     al,0x30
        mov     di,0

here:   cmp     byte[esi],0
        jnz     label5
        mov     esi,colors
label5: mov     ah,[esi]
        inc     esi
        call    get_index
        mov     [gs:ebx],ax
        inc     al
        mov     cx,0x194f
        sub     cx,dx
        mov     dx,cx
        call    get_index
        mov     [gs:ebx],ax
        inc     al
        cmp     al,0x3a
        jnz     label6
        mov     al,0x30
label6: mov     cx,0x194f
        sub     cx,dx
        mov     dx,cx
        call    move_point
        call    delay
        jmp     here
        
get_index:
        push    ax
        mov     al,80
        mul     dh
        xor     bx,bx
        add     bl,dl
        add     bx,ax
        pop     ax  
        ret
        
move_point:
        test    edi,0x1
        jnz     csub
        inc     dl
        jmp     cend
csub:   dec     dl
cend:   test    edi,0x2
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
        ret

rrev:   xor     edi,0x2
        jmp     rfin
crev:   xor     edi,0x1
        jmp     cfin

delay:
        push    ecx
        mov     ecx,0xffffff
label7: loop    label7
        pop     ecx
        ret


    
    	string1 db      "Running bootloader..."
    	string2 db      "BRH's character catapult 2.0"
        colors  db      0x09,0x0a,0x0c,0x0e,0x0f
                db      0x18,0x1c,0x1d,0x1e,0x1f
                db      0x58,0x59,0x5a,0x5e,0x5f 
                db      0x70,0x71,0x72,0x73,0x74
                db      0x30,0x3c,0x3d,0x3e,0x3f,0x00

        pgdt    dw      0
                dd      0x00008800

    	times 2560-($-$$) db 0
