#include <p24Fxxxx.h>
#include <xc.h>
#include <string.h>


#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)

// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled. 
					// Primary Oscillator refers to an external osc connected to the OSC1 and OSC2 pins)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // OSC2/CLKO/RC15 functions as port I/O (RC15)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))


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
    lcd_cmd(Address[y][x]);
}

void lcd_init(){
    wait(50);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    lcd_cmd(0b00111001); // function set, extended instruction mode
    lcd_cmd(0b00010100); // interval osc
    lcd_cmd(0b01110111); // contrast C3-C0
    lcd_cmd(0b01011101); // C5-C4, Ion, Bon
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

void setup(void)
{

CLKDIVbits.RCDIV = 0;

    AD1PCFG = 0xfffe; // AN0 is the only analog pin we are using
    TRISA = 1;
    // setup Timer 3
    T3CON = 0;
    TMR3 = 0;
    PR3 = 6250; //For 100ms
    T3CONbits.TCKPS = 3; // pre 1:256
    // we do not care about T3IF here. Don't reset it,
    // don't check it.
    
    T2CON = 0;
    TMR2 = 0;
    PR2 = 15625; //For 100ms
    T2CONbits.TCKPS = 2; // pre 1:256
    _T2IF = 0;
    
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
//strlen(s)
void lcd_printStr(const char *s) {
    int i;
    int j;
    
    for (i=0; i < strlen(s)-7; i++){
        lcd_setCursor(0,0);
        
        I2C2CONbits.SEN = 1; 				//Initiate Start condition
        while (I2C2CONbits.SEN);	 		// SEN will clear when Start Bit is complete
        _MI2C2IF = 0;

        I2C2TRN = 0b01111100; 			// 8-bits consisting of the slave address and the R/nW bit
        while (!IFS3bits.MI2C2IF);	
        _MI2C2IF = 0;
        
        for(j=0; j < 8; j++)
        {
            I2C2TRN = 0b11000000; 			// 8-bits consisting of control byte /w RS=1
            while (!IFS3bits.MI2C2IF);	
            _MI2C2IF = 0;

            I2C2TRN = *(s + (j+i)); 			// 8-bits consisting of the data byte
            while (!IFS3bits.MI2C2IF);
            _MI2C2IF = 0;
        }
        
        I2C2CONbits.PEN = 1;
        while (I2C2CONbits.PEN);				// PEN will clear when Stop bit is complete-
        _MI2C2IF = 0;
        
       // lcd_setCursor(0,0);
        wait(300);
    }
}




int B[1024];
volatile int adValue = 0;
char adStr[20];
void __attribute__((__interrupt__,__auto_psv__)) _ADC1Interrupt(void) 
{
    _AD1IF = 0;
    adValue = ADC1BUF0;
    sprintf(adStr, "%6.4f V", (3.3/1024)*adValue); // ?x.xxxx V?
    // 6.4 in the format string ?%6.4f? means 6 placeholders for the whole
    // floating point number, 4 of which are for the fractional part.
    lcd_printStr(adStr);
    
}



int main(void) {
    
    setup();   
    lcd_init();
    int i = 0;   
   lcd_printStr("Do not go gentle into that good night, Old age should burn and rave at close of day; Rage, rage against the dying of the light.");
    
    wait(1000);
    while(1)
    {
        while(!_T2IF);
        _T2IF = 0;
        
        B[i++] = adValue;
        i %= 1024; 
        
    }
   
    return 0;
}