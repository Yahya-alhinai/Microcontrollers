#include <p24Fxxxx.h>
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// 1 ms delay function
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


void lcd_setCursor(int x, int y){
    
    int Address[2][8] = {{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87}, 
                         {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7}
    };
    lcd_cmd(Address[x][y]);
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

    wait(300);
}


void setup(void)
{

CLKDIVbits.RCDIV = 0;

    AD1PCFG = 0xfffe; // AN0 is the only analog pin we are using
    TRISA = 1;
    
    // setup Timer 2
    T2CON = 0;
    TMR2 = 0;
    PR2 = 62500; //For 1s
    T2CONbits.TCKPS = 3; // pre 1:256
    _T2IP = 2;  // set it as low priority interrupt
    _T2IE = 1;
    _T2IF = 0;

    // we do not care about T3IF here. Don't reset it,
    // don't check it.
    
    T3CON = 0;
    TMR3 = 0;
    PR3 = 15625; 
    T3CONbits.TCKPS = 1; // pre 1:256
    _T3IF = 0;
    
    
    // Setup on A/D
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CHS = 0;
    
    AD1CON1bits.ASAM = 1; // auto-sampling. P21, Sec. 17.7.3.3 in the ADC
       // document says use ASAM=1
    AD1CON1bits.SSRC = 0b010; // Use Timer3 as the conversion trigger
       // We can also use Timer3 / Timer2 as a 32-bit timer to trigger
       // A/D sampling/conversion, because the 32-bit timer
       // will also generate the T3IF event.
    
    //doesn't matter //AD1CON3bits.SAMC = 3; // auto-sample time = X * TAD (what is X?)
    AD1CON3bits.ADCS = 2; // clock = Y * Tcy (what is Y?)
    

    // Configure A/D interrupt
    _AD1IF = 0; // clear flag           
    _AD1IE = 1; // enable interrupt

    AD1CON1bits.ADON = 1;   //Turn on ADC

    I2C2CON = 0;
    I2C2BRG = 157;
    I2C2CONbits.I2CEN = 1;
    _MI2C2IF = 0;
    
    // turn on Timer 2 and 3
    T3CONbits.TON = 1;
    T2CONbits.TON = 1;
    
 }
