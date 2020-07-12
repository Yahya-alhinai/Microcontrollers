/*
 * File:   lcd_2_8.c
 * Author: dali9
 *
 * Created on March 31, 2018, 11:57 AM
 */



#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include <p24Fxxxx.h>
#include <string.h>



void delay_for_1msFunction(void){
    int i;
    for (i=0 ; i<140 ; i++) {
        asm("nop");
    }   
}
void delay_for_21usFunction(void){
    int i;
    for (i=0 ; i<1 ; i++) {
        asm("nop");
    }   
}
void delay1ms(unsigned int i){
    unsigned int j;
    for (j=0 ; j<i ; j++) {
        delay_for_1msFunction();
    }     
}
void lcd_cmd(char Package) {
    I2C2CONbits.SEN = 1; //Initiate Start condition 
    while(I2C2CONbits.SEN==1); // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b00000000; // 8-bits consisting of control byte
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = Package; // 8-bits consisting of the data byte
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.PEN =1;
    while(I2C2CONbits.PEN==1); // PEN will clear when Stop bit is complete
    IFS3bits.MI2C2IF = 0;
    // write delay of one cycle for LCD in 100kHz
    delay_for_21usFunction();
    //asm("nop");
}
void lcd_init(void){
    //wait 50ms;
    delay1ms(50);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    lcd_cmd(0b00111001); // function set, extended instruction mode
    lcd_cmd(0b00010100); // interval osc
    lcd_cmd(0b01110111); // contrast C3-C0
    lcd_cmd(0b01011101); // C5-C4, Ion, Bon
    lcd_cmd(0b01101100); // follower control
    //wait 200ms;
     delay1ms(200);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    lcd_cmd(0b00001100); // Display On
    lcd_cmd(0b00000001); // Clear Display
    delay1ms(2);
    //asm("nop");
}
void lcd_setCursor(char x, char y){
    unsigned char Cursor = 0b00000000;
    Cursor =  (0x40*x) + y; // x is number of raw; in this case x=0 or x=1 // y is number of column {0,1,2,..,7} See page 13 of background lab 5
    Cursor = (0b10000000 | Cursor); // "OR" with bit8=1 because it's address see page 13 the last paragraph of the background lab 5
    lcd_cmd(Cursor);
}

void setuplcd(void){
    CLKDIVbits.RCDIV = 0; //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0x9fff;
    //TRISA=0;
    //TRISB=0;
    //TRISBbits.TRISB2=1;
    //TRISBbits.TRISB3=0;
    //CNPU1bits.CN7PUE=1; //Enable Pull-up resistor for RB3
    //LATA=1;
    //LATB=1;
    //PORTB=1;
    TRISBbits.TRISB5=0;
    
    
    I2C2CON=0;
    I2C2BRG = 157;        // baud rate=156.08
    I2C2CONbits.SCLREL=1; // Releases SCLx clock seem page 154 of data sheet PIC24         
    I2C2CONbits.I2CEN=1;  // enables I2c
    I2C2CONbits.RCEN=1;   // Enable Receive 
    IFS3bits.MI2C2IF = 0; // clear flag
    //IEC3bits.MI2C2IE=1;
    

    
    
    TMR1  = 0x0000;    
    PR1   = 12500;  
    T1CON = 0x8020; // PRE =1:64
    _T1IF = 0;
    
    TMR2  = 0x0000;    
    PR2   = 159;  
    T2CON = 0x8000; // PRE =1:1
    _T2IF = 0;
}
void lcd_printChar(char Package){
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.SEN = 1; //Initiate Start condition 
    while(I2C2CONbits.SEN==1); // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01000000; // 8-bits consisting of control byte
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = Package; // 8-bits consisting of the data byte
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    
    I2C2CONbits.PEN =1;
    while(I2C2CONbits.PEN==1); // PEN will clear when Stop bit is complete
    IFS3bits.MI2C2IF = 0;
    delay_for_21usFunction();
    asm("nop");    
}
void lcd_printStr(const char *s){
    char i=0;
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.SEN = 1; //Initiate Start condition 
    while(I2C2CONbits.SEN==1); // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    for(i=0;i<strlen(s);i++){
        I2C2TRN = 0b11000000; // 8-bits consisting of control byte
        while(IFS3bits.MI2C2IF == 0);
        IFS3bits.MI2C2IF = 0;
        I2C2TRN = *(s+i); // 8-bits consisting of the data byte
        while(IFS3bits.MI2C2IF == 0);
        IFS3bits.MI2C2IF = 0;  
    }
    I2C2CONbits.PEN =1;
    while(I2C2CONbits.PEN==1); // PEN will clear when Stop bit is complete
    IFS3bits.MI2C2IF = 0;
    delay_for_21usFunction();
    asm("nop");  
}
void lcd_printStrM(const char *s){
    lcd_setCursor(0,0);
    int i;
    for(i=0;i<strlen(s);i++){
    char j=0;
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.SEN = 1; //Initiate Start condition 
    while(I2C2CONbits.SEN==1); // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    for(j=0; j<8; j++){
        I2C2TRN = 0b11000000; // 8-bits consisting of control byte
        while(IFS3bits.MI2C2IF == 0);
        IFS3bits.MI2C2IF = 0;
        I2C2TRN = *(s+(i+j)%(strlen(s)+1)); // 8-bits consisting of the data byte
        while(IFS3bits.MI2C2IF == 0);
        IFS3bits.MI2C2IF = 0;  
    }
    I2C2CONbits.PEN =1;
    while(I2C2CONbits.PEN==1); // PEN will clear when Stop bit is complete
    IFS3bits.MI2C2IF = 0;
    delay_for_21usFunction();
    asm("nop");
    delay1ms(7000);
    lcd_setCursor(0,0);
    }
    }
    


/*
void lcd_printStrMov1(const char *s){
    lcd_setCursor(0,0);
    char i=0;
    char p=0;
    char size;
    static char copy=0;
    size=(strlen(s));
    char x [(size+1)];
    if(copy==0){
        for(i=0;i<(size+1);i++){
        x[i]=*(s+i);
        }   
        copy=1;
    }         
    for(i=0;i<8;i++){
        lcd_printChar(x[i]);
    }
    for(i=0;i<(size+1);i++){
        p=x[0];
        x[i]=x[1+i];
    }
    x[i]= x[0];
}
*/
/*
void lcd_printStrMov(const char *s){
    char i=0;
    char p=0;
    for(i=0;i<strlen(s);i++){
        lcd_setCursor(0,(strlen(s)-p));
        lcd_printChar(*(s+i));

    }
}
*/
