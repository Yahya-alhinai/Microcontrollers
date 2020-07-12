#include <p24Fxxxx.h>
#include <xc.h>
#include <stdlib.h>

#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled. 

#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // OSC2/CLKO/RC15 functions as port I/O (RC15)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

volatile unsigned char buffer[32];
volatile unsigned char front = 0;
volatile unsigned char back = 0;


void setup(void)
{
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;
    TRISB = 0;

    // add your configuration commands below
    PORTB = 0xffff;
    
    // Can't simulate it in MPLAB
    TRISBbits.TRISB4 = 0; // SCK
    TRISBbits.TRISB5 = 0; // SDO
    TRISBbits.TRISB6 = 0; // this will be the DAC /LDAC. I assume /CS is hardwired to GND

    // Peripheral Pin Select 
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock
    RPOR2bits.RP5R = 7;   //RB5->SPI1:SDO1; See Table 10-3 on P109 of the datasheet
    RPOR2bits.RP4R = 8;   //RB4->SPI1:SCK1OUT;
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock
    
    SPI1CON1 = 0;
    SPI1CON1bits.MSTEN = 1;
    SPI1CON1bits.MODE16 = 1;
    SPI1CON1bits.CKE = 0;
    SPI1CON1bits.CKP = 0;
    
    //As fast as possible
    SPI1CON1bits.SPRE = 0b110;
    SPI1CON1bits.PPRE = 0;  
    SPI1CON2 = 0;
    SPI1STAT = 0;
    SPI1CON1bits.SPRE = 0b110; // secondary = 2
    SPI1CON1bits.PPRE = 0b11;  // primary = 1;
    
    T2CON = 0;
    PR2 = 15999;
    T2CONbits.TON = 1;
    IFS0bits.T2IF = 0;
    TMR2 = 0;
    _SPI1IF = 0;
}

int main(int argc, char *argv[])
{
    setup();
    int Temp;
    
    while (1){
        SPI1BUF = 0x7f92;
        while(_SPI1IF == 0);
        _SPI1IF == 0;
        
        TMR2 = 0;
        while (_T2IF == 0);
        _T2IF == 0;
        
        SPI1BUF = -1;
        while(_SPI1IF == 0);
        _SPI1IF == 0;
        Temp = SPI1BUF;  // junk value
    }

    return 0; 
}

