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
#pragma config POSCMOD = NONE      // Primary Oscillator Select (Primary oscillator disabled. 
                                   // Primary Oscillator refers to an external osc connected to the OSC1 and OSC2 pins)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // OSC2/CLKO/RC15 functions as port I/O (RC15)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                   // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))



void __attribute__((__interrupt__,__auto_psv__)) _T1Interrupt(void)
{
    static unsigned int k = 0;
    LATBbits.LATB2 = 0;
    
    if ((k++ % 60) == 0)
        LATBbits.LATB2 = 1;
    
    _T1IF = 0;
}



void setup(void) {
    CLKDIVbits.RCDIV = 0;

    AD1PCFG = 0x9fff; // all digital
    TRISB = 0;
    LATB = 0;
    
    T1CON = 0x0030; // turn off timer, prescalar 1:1, use Tcy as clk source
    PR1 = 62500;
    TMR1 = 0;
    _T1IF = 0; // equivalent to IFS0bits.T1IF = 0;
    _T1IE = 1;
    T1CONbits.TON = 1; // now turn on the timer
}

int main(void) {

    setup();

    while (1);

}