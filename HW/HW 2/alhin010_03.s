    
_main:
    bclr    CLKDIV,#8                 
    mov     #stack,w15                ;initialize the stack pointer
    mov     #0x9fff,w0
    mov     w0,AD1PCFG                ;PORTB will be digital!
    
    
    
    mov #AList, w1
    mov [w1++], w6
    mov w6, MinVal
    
    mov #19 ,w3
    loop:
    mov MinVal, w2
    
    CP w2, [w1]
    bra n, MinVal_Is_Min
    
    mov [w1], w2
    mov w2, MinVal
    
    MinVal_Is_Min:
    
    inc2 WREG1
    dec WREG3
    
    bra nz, loop