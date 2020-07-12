
Setup_W0:
    clr w0		  // Make sure w0 is set to zero
    
    CP w10, w11		  // If w10 is less then w11 then branch to Adding. Otherwise, continue to the second condition
    bra N, Adding

    CP w10, w12		  // If w10 is less then w11 then branch to Adding. Otherwise, do the following computation (w0 = w11 + w12) 
    bra N, Adding
    
    add w11, w12, w0	   // If w10 is NOT less then w11 and w12 this instruction will be executed and retuen the value of w0
    return
    
    Adding:
	mov w10, w0	   // If either w11 or w12 is more than or equal t w0, this instruction will be executed and retuen the value of w0
	
return   

    
_main:
    bclr    CLKDIV,#8                 
    mov     #stack,w15                ;initialize the stack pointer
    mov     #0x9fff,w0
    mov     w0,AD1PCFG                ;PORTB will be digital!
    
    call Setup_W0