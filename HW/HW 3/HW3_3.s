
	.equ __P24FJ64GA002,1        ;required "boiler-plate" (BP)
        .include "p24Fxxxx.inc"      ;BP

#include "xc.inc"                    ;BP
        ;the next two lines set up the actual chip for operation - required
        config __CONFIG2, POSCMOD_NONE & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSECME & FNOSC_FRCPLL & SOSCSEL_LPSOSC & WUTSEL_FST & IESO_OFF
        config __CONFIG1, FWDTEN_OFF & BKBUG_OFF & GWRP_OFF & GCP_OFF & JTAGEN_OFF

        
    .bss                        ;put the following labels in RAM
AStr: .space 30  ; AStr is a variable of type ?string?, similar to the C definition: char AStr [30] 
ALen: .space 2 
Bstr: .space 30 
BLen: .space 2            ;this will be our stack area
    .text                       ;BP (put the following data in ROM(program memory))
    .global _main               ;BP

_main:
    
    mov #AStr ,w8
    mov.b #'a', w6
    mov.b w6, [w8++]
    mov.b #'f'+1, w6 
    mov.b w6, [w8++]
    mov.b #'@', w6 
    mov.b w6, [w8++]
    mov.b #'?', w6 
    mov.b w6, [w8++]
    mov.b #'F'+6, w6 
    mov.b w6, [w8++]
    mov.b #'a', w6 
    mov.b w6, [w8++]
    mov.b #'z', w6 
    mov.b w6, [w8++]
    mov.b #'f', w6 
    mov.b w6, [w8++]
    mov.b #'a'+1, w6 
    mov.b w6, [w8++]
    mov.b #'A'+3, w6 
    mov.b w6, [w8++]
    mov.b #'A'+4, w6 
    mov.b w6, [w8++]
    mov.b #'A'+5, w6 
    mov.b w6, [w8++]
    mov.b #0+6, w6 
    mov.b w6, [w8++]
    mov.b #'n', w6 
    mov.b w6, [w8++]
    mov.b #'A'+6, w6 
    mov.b w6, [w8++]
    mov.b #'A'+6, w6 
    mov.b w6, [w8++]
    

    MOV #AStr, w0
    MOV #96, w1 
    MOV #123, w2
    
    toUpper:
	MOV.B [w0], w5
	CPSGT.B w5, w1
	    BRA CarryOn
	CPSLT.B w5, w2
	    BRA CarryOn
	    
	SUB.B #32, w5
	MOV.B w5, [w0]

	CarryOn:
	CP0.B [w0++]
	BRA NZ, toUpper

    
    .end

