    
_main:
    bclr    CLKDIV,#8                 
    mov     #stack,w15                ;initialize the stack pointer
    mov     #0x9fff,w0
    mov     w0,AD1PCFG                ;PORTB will be digital!
    
    
    clr w0
    mov #AVec, w6
    mov #BVec, w7
    
    mov #5 ,w5
    loop:
	mov [w6++], w11
	mul.ss w11, [w7++], w0
	add Sum

	dec WREG5
    bra nz, loop