section .data
		number  dd 42145,647,634562,356326,8568,523,252,5685,6745,353,53253,252,2352,325325,3532535,35353,534534,353,359,44,0
		output  db 0,0,0,0,0,0,0,0,0,0,' '

section .text
global main
main:   lea     esi,[number]
        mov     ecx,0x14
        mov     ebx,0
        cld
label:  
        lodsd
        cmp     eax,ebx
        jng     small
        mov     ebx,eax
small:  loop    label
        
        mov     ecx,9
        lea     edi,[output]
        add     edi,8
        std
        mov     edx,ebx
        mov     ebx,10
make:   mov     eax,edx
        mov     edx,0
        div     ebx
        xchg    eax,edx
        add     eax,0x30
        stosb
        cmp     dx,0
        jz      break
        loop    make
break:  
        mov     edx,12
        sub     edx,ecx
        mov     ecx,edi
        inc     ecx
        mov     ebx,1
        mov     eax,4
        int     0x80
        
        mov     ebx,0
        mov     eax,1
        int     0x80
    


