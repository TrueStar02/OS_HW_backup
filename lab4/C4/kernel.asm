
[bits 32]

        mov	    ecx,1000
        mov	    ax,0
        mov         ebx,0
label5:	mov	    [gs:ebx],ax
	inc	    ebx
	inc	    ebx
	loop	    label5

        mov     dx,0x0100
        mov     esi,colors
        mov     al,0x30
        mov     edi,0

here3:  cmp     byte[esi],0
        jnz     label6
        mov     esi,colors
label6: mov     ah,[esi]
        inc     esi
        call    print_title
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
        jnz     label7
        mov     al,0x30
label7: mov     cx,0x194f
        sub     cx,dx
        mov     dx,cx
        call    move_point
        call    delay
        jmp     here3

print_title:
        push    esi
        push    ax
        mov     ebx,52
        mov     ecx,28
        mov     esi,string3
        
label8: lodsb
        mov     [gs:ebx],ax
        inc     ebx
        inc     ebx
        loop    label8
        
        pop     ax
        pop     esi
        ret

get_index:
        push    ax
        mov     al,160
        mul     dh
        xor     ebx,ebx
        add     bl,dl
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
cfin:   ret

rrev:   xor     edi,0x2
        jmp     rfin
crev:   xor     edi,0x1
        jmp     cfin

delay:
        push    ecx
        mov     ecx,0x3fffff
label9: loop    label9
        pop     ecx
        ret

        string3 db      "BRH's character catapult 3.0"
        colors  db      0x09,0x0a,0x0c,0x0e,0x0f
                db      0x18,0x1c,0x1d,0x1e,0x1f
                db      0x58,0x59,0x5a,0x5e,0x5f 
                db      0x70,0x71,0x72,0x73,0x74
                db      0x30,0x3c,0x3d,0x3e,0x3f,0x00