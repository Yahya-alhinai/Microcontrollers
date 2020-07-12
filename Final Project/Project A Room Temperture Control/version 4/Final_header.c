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

void setup(void)
{
    CLKDIVbits.RCDIV = 0; //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0x9ffe;

    //------------PINS SETUP------------//
    _TRISA1 = 1;
    _TRISA2 = 1;
    _TRISA3 = 1;
    _TRISA4 = 1;
    _TRISB4 = 1;
    _TRISB13 = 1;

    _CN3PUE  = 1;   // RA1 pull up
    _CN30PUE = 1;   // RA2 pull up
    _CN29PUE = 1;   // RA3 pull up
    _CN0PUE  = 1;   // RA4 pull up
    _CN1PUE  = 1;   // RB4 pull up
    _CN13PUE = 1;   // RA13 pull up

    //-----(FAN-HEATER-COOLER) OUTPUTS------//
    _TRISB5 = 0;
    _TRISB6 = 0;
    _TRISB7 = 0;
    _LATB5 = 0;
    _LATB6 = 0;
    _LATB7 = 0;
    //----------------------------------//
    
    
    //------------LCD SETUP------------//
    I2C2CON = 0;
    I2C2BRG = 157;
    I2C2CONbits.I2CEN = 1;
    _MI2C2IF = 0;
    //---------------------------------//
    
    
    //------SCREEN REFRESH RATE (TMR1)--------//
    TMR1 = 0;
    PR1 = 18750; //0.3 s 
    _T1IF = 0;
    T1CON = 0x8030;
    IEC0bits.T1IE = 1;
    //---------------------------------//

    
    //-----------SAMPLING -------------//
    //SAMPLING RATE
    T3CON = 0;
    TMR3 = 0;
    PR3 = 18750;
    T3CON = 0x8030;

    // Setup on A/D
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CHS = 0;

    AD1CON1bits.ASAM = 1;
    AD1CON1bits.SSRC = 0b010;
    AD1CON3bits.ADCS = 2;

    _AD1IF = 0;
    _AD1IE = 1;

    AD1CON1bits.ADON = 1;
    //---------------------------------//
    
    
    
    //--------LCD_INIT-----------------//
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
    
    //---------------------------------//
    
    
    
    //-----------Bluetooth Setup ------//
    U1MODE = 0;
    U1MODEbits.BRGH = 1;
    U1BRG = 416; // 9600 baud
    U1MODEbits.UEN = 0;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;

    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    _RP14R = 0x0003;   //RB6->UART1:U1TX; See Table 10-3 on P109 of the datasheet
    _U1RXR = 15;   //RB10->UART1:U1RX;
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS

    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;
    //---------------------------------//
    
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
    
    wait(1);

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