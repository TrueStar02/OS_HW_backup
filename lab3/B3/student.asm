%include "head.include"

your_if:
		mov		eax,[a1]
		cmp 	eax,12
		jge		label1
		shr		eax,1
		inc 	eax
		jmp		exits
label1:	cmp		eax,24
		jge		label2
		mov		bx,24
		sub		bx,ax
		mul 	bx
		jmp		exits
label2:	shl		eax,4
exits:	mov		[if_flag],eax	

your_while:
		mov 	ebx,[while_flag]
		mov		ecx,[a2]
label3: cmp		ecx,12
		jl		break
		call	my_random
		mov		edi,ecx
		sub		edi,12
		mov		[edi+ebx],eax
		dec		ecx
		jmp		label3
break:	mov		[a2],ecx

%include "end.include"

your_function:
		pusha
		mov		esi,[your_string]
label4:	
 		lodsb
		cmp		al,0
		jz		stop
		push	ax
		call	print_a_char
		pop		ax
		jmp		label4
stop:	popa
		ret