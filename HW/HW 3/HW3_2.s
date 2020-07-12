        .equ __P24FJ64GA002,1        ;required "boiler-plate" (BP)
        .include "p24Fxxxx.inc"      ;BP

#include "xc.inc"                    ;BP
        ;the next two lines set up the actual chip for operation - required
        config __CONFIG2, POSCMOD_NONE & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSECME & FNOSC_FRCPLL & SOSCSEL_LPSOSC & WUTSEL_FST & IESO_OFF
        config __CONFIG1, FWDTEN_OFF & BKBUG_OFF & GWRP_OFF & GCP_OFF & JTAGEN_OFF



.bss 
AStr: .space 30  ; AStr is a variable of type ?string?, similar to the C definition: char AStr [30] 
ALen: .space 2 
Bstr: .space 30 
BLen: .space 2         ;this will be our stack area, from var_16_bit to here
 
    .text                       ;BP (put the following data in ROM(program memory))
    .global _main  


cFoundInStr:
    MOV.b [w0], w2
    CPSNE.B w2,w1
	BRA Set_C_to_one

    cp0.b [w0++]
    bra NZ, cFoundInStr

    CLR w0
    return

    Set_C_to_one:
	MOV #1, w0
return
    
    
_main:
    
    mov #AStr ,w8
    mov.b #'A', w6 
    mov.b w6, [w8++]
    mov.b #'A'+1, w6 
    mov.b w6, [w8++]
    mov.b #'A'+3, w6 
    mov.b w6, [w8++]
    mov.b #'A'+4, w6 
    mov.b w6, [w8++]
    mov.b #'A'+5, w6 
    mov.b w6, [w8++]
    mov.b #'A'+6, w6 
    mov.b w6, [w8++]
    mov.b #'n', w6 
    mov.b w6, [w8++]
    mov.b #'A'+6, w6 
    mov.b w6, [w8++]
    mov.b #'A'+6, w6 
    mov.b w6, [w8++]
    

    MOV #AStr, w0
    MOV #'C', W1

    call cFoundInStr

    nop    
    
.end