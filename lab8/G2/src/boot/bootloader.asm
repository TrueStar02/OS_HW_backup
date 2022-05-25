		org		0x7e00
		
[bits 16]
	    
        mov     ax,0xb800
        mov     es,ax
        mov     si,string1

        mov     cx,21
        mov     di,1600
        mov     ah,0xa
label2: lodsb
        stosw
        loop    label2
        
        mov     ah,0x86
        mov     cx,15
        mov     dx,0
        int     0x15

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
        mov     eax,0x8
        mov     ds,eax
        mov     es,eax
        mov     eax,0x10
        mov     ss,eax
        mov     eax,0x18
        mov     gs,eax
        
        mov     ah,0xe
        mov     ebx,1920
        mov     ecx,26
        mov     esi,string2

label3: lodsb
        mov     [gs:ebx],ax
        inc     ebx
        inc     ebx
        loop    label3

        mov     ecx,0xffffff
label4: loop    label4

        mov     eax,26
        push    eax
        mov     eax,6
        push    eax
        mov     edi,0x20000
        push    edi
        call    read_disk
        add     esp,12

        jmp     dword 0x0020:0x20000

here2:  jmp     here2

read_disk:
        push    ebp
        mov     ebp,esp
        push    eax
        push    edi
        push    ecx
        push    edx

        mov     eax,[bp+4*3]
        mov     edx,0x1f3
        out     dx,ax

        inc     edx
        inc     edx
        xor     eax,eax
        or      ax,0xe000
        out     dx,ax

        mov     edx,0x1f2
        mov     eax,[ebp+4*4]
        mov     ecx,eax
        out     dx,al

		mov		edx,0x1f7        
        mov     edi,[ebp+4*2]
        mov		al,0x20
        out		dx,al

rd2:	push	ecx
		mov		edx,0x1f7
label:  in      al,dx
        and     al,0x89
        cmp     al,0x08
        jnz     label


        mov		ecx,256
        mov     edx,0x1f0  
rep     insw
		pop		ecx
		loop	rd2
        pop     edx
        pop     ecx
        pop     edi
        pop     eax
        pop     ebp
        ret
    
    	string1 db      "Running bootloader..."
        string2 db      "Running in protect mode..."

        pgdt    dw      0
                dd      0x00008800
