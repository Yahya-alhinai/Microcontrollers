
#include <p24Fxxxx.h>
#include <xc.h>

// PIC24FJ64GA002 Configuration Bit Settings
// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
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



void init7seg(void){
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;
    
    //TRISA = 0b0000000000011111;
    TRISB = 0b1111000000000011;
    
    LATA = 0xFFFF;
    LATB = 0x0000;

}

enum DIGIT {
   ZERO     = 0b0000000000001100,
   ONE      = 0b0000001001111100,
   TWO      = 0b0000000010010100,
   THREE    = 0b0000000000110100,
   FOUR     = 0b0000001001100100, 
   FIVE     = 0b0000000100100100, 
   SIX      = 0b0000000100000100,  
   SEVEN    = 0b0000000001111100,
   EIGHT    = 0b0000000000000100,
   NINE     = 0b0000000001100100,
   A        = 0b0000000001000100,
   B        = 0b0000001100000100,
   C        = 0b0000000110001100,
   D        = 0b0000001000010100,
   E        = 0b0000000110000100,
   F        = 0b0000000111000100,
           
};

void showChar7seg(char myChar, enum DIGIT myDigit){
    if (myChar)
        LATB = myDigit | 0b0000100000000000;    
    else
        LATB = myDigit | 0b0000010000000000;
    
    
};

void delay(){
    long long int i = 200000;
        while (i--);

}

int main(void) {
    /**/
    init7seg();
    
    while(1){
        
        showChar7seg(0, ZERO);
        delay();
        showChar7seg(1, ONE);
        delay();
        showChar7seg(0, TWO);
        delay();
        showChar7seg(1, THREE);
        delay();
        showChar7seg(0, FOUR);
        delay();
        showChar7seg(1, FIVE);
        delay();
        showChar7seg(0, SIX);
        delay();
        showChar7seg(1, SEVEN);
        delay();
        showChar7seg(0, EIGHT);
        delay();
        showChar7seg(1, NINE);
        delay();
        showChar7seg(0, A);
        delay();
        showChar7seg(1, B);
        delay();
        showChar7seg(0, C);
        delay();
        showChar7seg(1, D);
        delay();
        showChar7seg(0, E);
        delay();
        showChar7seg(1, F);
        delay();
        
    }
    
    return 0;
}
