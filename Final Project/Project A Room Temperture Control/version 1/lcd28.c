#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void wait(int number){
    int i;
    for (i = 0; i < number; i++){
        int long k = 940;
        while (k--);
    }
}

void lcd_cmd(char Package) {
    
    I2C2CONbits.SEN = 1; 				//Initiate Start condition
    while (I2C2CONbits.SEN);			//Wait for SEN == 0 // SEN will clear when Start Bit is complete
    _MI2C2IF = 0;
    
    I2C2TRN = 0b01111100; 			// 8-bits consisting of the slave address and the R/nW bit
    while (!IFS3bits.MI2C2IF);  //Wait for IFS3bits.MI2C2IF == 1
    _MI2C2IF = 0;
    I2C2TRN = 0b00000000; 			// 8-bits consisting of control byte
    while (!IFS3bits.MI2C2IF);	//Wait for IFS3bits.MI2C2IF == 1
    _MI2C2IF = 0;
    I2C2TRN = Package;              // 8-bits consisting of the data byte
    while (!IFS3bits.MI2C2IF);	//Wait for IFS3bits.MI2C2IF == 1
    
    I2C2CONbits.PEN = 1;
    while (I2C2CONbits.PEN);			//Wait for PEN==0 // PEN will clear when Stop bit is complete
    _MI2C2IF = 0;

}


void lcd_init(){
    wait(50);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    lcd_cmd(0b00111001); // function set, extended instruction mode
    lcd_cmd(0b00010100); // interval osc
    lcd_cmd(0b01110000); // contrast C3-C0
    lcd_cmd(0b01011110); // Ion, Bon, C5-C4
    lcd_cmd(0b01101100); // follower control
    wait(200);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    lcd_cmd(0b00001100); // Display On
    lcd_cmd(0b00000001); // Clear Display
    wait(2);
}

void lcd_setCursor(char x, char y){
    static int Cursor;
    
    Cursor =  (0x40*x) + y; // x is number of raw; in this case x=0 or x=1 // y is number of column {0,1,2,..,7} See page 13 of background lab 5
    Cursor = (0b10000000 | Cursor); // "OR" with bit8=1 because it's address see page 13 the last paragraph of the background lab 5
    lcd_cmd(Cursor);
}


void lcd_printChar(char Package) {
    I2C2CONbits.SEN = 1; 				//Initiate Start condition
    while (I2C2CONbits.SEN);	 		// SEN will clear when Start Bit is complete
    _MI2C2IF = 0;
    
    I2C2TRN = 0b01111100; 			// 8-bits consisting of the slave address and the R/nW bit
    while (!IFS3bits.MI2C2IF);	
    _MI2C2IF = 0;
    
    I2C2TRN = 0b01000000; 			// 8-bits consisting of control byte /w RS=1
    while (!IFS3bits.MI2C2IF);	
    _MI2C2IF = 0;
    
    I2C2TRN = Package; 			// 8-bits consisting of the data byte
    while (!IFS3bits.MI2C2IF);
    _MI2C2IF = 0;
    
    I2C2CONbits.PEN = 1;
    while (I2C2CONbits.PEN);				// PEN will clear when Stop bit is complete-
    _MI2C2IF = 0;
    
    }


void lcd_printStr(const char *s) {
    int i;
    
    I2C2CONbits.SEN = 1; 				//Initiate Start condition
    while (I2C2CONbits.SEN);	 		// SEN will clear when Start Bit is complete
    _MI2C2IF = 0;

    I2C2TRN = 0b01111100; 			// 8-bits consisting of the slave address and the R/nW bit
    while (!IFS3bits.MI2C2IF);	
    _MI2C2IF = 0;

    for (i=0; i < strlen(s); i++){
        I2C2TRN = 0b11000000; 			// 8-bits consisting of control byte /w RS=1
        while (!IFS3bits.MI2C2IF);	
        _MI2C2IF = 0;

        I2C2TRN = *(s + i); 			// 8-bits consisting of the data byte
        while (!IFS3bits.MI2C2IF);
        _MI2C2IF = 0;
    }

    I2C2CONbits.PEN = 1;
    while (I2C2CONbits.PEN);				// PEN will clear when Stop bit is complete-
    _MI2C2IF = 0;

}