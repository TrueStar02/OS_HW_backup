section .data
		number  dw 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		output  db 0,0,0,0,0,0,0,0,0,0,' '

section .text
global main
main:   lea     edi,[number]
        add     edi,4
        mov     ecx,0x12
        mov     ebx,0
        cld
label:  mov     eax,0
        add     eax,[edi-4]
        add     eax,[edi-2]
        stosw
        loop    label
        
        mov     eax,0
        mov     ecx,20
        lea     esi,[number]

print:  lodsw
        mov     edx,eax
        push    ecx
        mov     ecx,9
        lea     edi,[output]
        add     edi,8
        std
        mov     ebx,10
make:   mov     eax,edx
        mov     edx,0
        div     ebx
        xchg    eax,edx
        add     eax,0x30
        stosb
        cmp     edx,0
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
        pop     ecx
        cld
        loop    print

        mov     ebx,0
        mov     eax,1
        int     0x80
    


