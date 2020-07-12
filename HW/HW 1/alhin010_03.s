
setup:
    mov     #0b0000000000000000, w0
    mov     w0, TRISB
    
    mov     #0x9fff,w0
    mov     w0,AD1PCFG
    
    bclr    CLKDIV,#8
    
    call pingPong
return
    
    
pingPong:
    clr LATB
    inc LATB
	 
    mov #15, w5
    left_Shift:
	call wait1s
	SL LATB
	dec WREG5
    bra nz, left_Shift
    
    mov #15, w5
    right_Shift:
	call wait1s
	LSR LATB
	dec WREG5
    bra nz, right_Shift
    
    mov #15, w5
    bra left_Shift  
return    


