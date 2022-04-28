[bits 32]
global  func_from_asm
extern  func_from_c
extern  func_from_cpp  

func_from_asm:
    call    func_from_c
    call    func_from_cpp
    ret