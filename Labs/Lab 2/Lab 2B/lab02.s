.include "xc.inc" ; required "boiler-plate" (BP)
    
;the next two lines set up the actual chip for operation - required
config __CONFIG2, POSCMOD_EC & I2C1SEL_SEC & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSECME & FNOSC_FRCPLL & SOSCSEL_LPSOSC & WUTSEL_FST & IESO_OFF

config __CONFIG1, WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & BKBUG_ON & GWRP_ON & GCP_ON & JTAGEN_OFF

    .bss ; put the following labels in RAM
counter:
 .space 2 ; a variable that takes two bytes (we won?t use
 ; it for now, but put here to make this a generic
 ; template to be used later).
stack:
 .space 32 ; this will be our stack area, needed for func calls
.text ; BP (put the following data in ROM(program memory))
;because we are using the C compiler to assemble our code, we need a "_main" label
;somewhere. (There's a link step that looks for it.)
 
 
write_bit_stream: 
    repeat #14 ;1
    nop	       ;15
    clr LATA   ;1
    repeat #29 ;1
    nop	       ;30
    inc LATA   ;1
return     ;3

delays_1_us:
    repeat #9
    nop
return 
 
delays_10_us:
    repeat #153
    nop
return 
  
    
delays_100_us:
    repeat #1593
    nop
return 
 
delays_1_ms:
    repeat #15993
    nop
return  
    
    
    
write_0:      ;total 20
    inc LATA  ;1
    repeat #3 ;1
    nop	      ;4
    
    clr LATA  ;1
    repeat #7 ;1
    nop       ;8
return        ;3
    

write_1:       ;total 20     
    inc LATA   ;1
    repeat #9  ;1
    nop        ;10
    
    clr LATA   ;1
    repeat #2  ;1
    nop        ;3
return         ;3

    

 
.global _main ;BP
_main:
    
    bclr CLKDIV,#8 ;BP
    nop

    mov #0x9fff,w0
    mov w0,AD1PCFG			; Set all pins to digital mode
    mov #0b1111111111111110,w0
    mov w0,TRISA			; set pin RA0 to output
    mov #0x0001,w0
    mov w0,LATA				 ; set pin RA0 high
    
foreverLoop:
    call delays_1_ms
    
    ;000000010000000100000000 very dim red and green
    ;000000001000000010000000 bright green and blue
    
    
    
    ;RED
    call write_1
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    
    ;GREEN
    call write_1
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    

    ;BLUE
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0
    call write_0

    
;    call write_bit_stream
;    call delays_1_us      
;    call delays_10_us 
;    call delays_100_us 
;    call delays_1_ms
    
;    bra foreverLoop
.end
    
    
    