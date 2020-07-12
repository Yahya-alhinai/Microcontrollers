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


volatile int value = 0;
void __attribute__((__interrupt__,__no_auto_psv__)) _ADC1Interrupt(void) {
    _AD1IF = 0;
    value = ADC1BUF0;
}

void setup(void) {
    CLKDIVbits.RCDIV = 0;

    AD1PCFG = 0xfffe;

    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CHS = 0;

    AD1CON1bits.ASAM = 1; // auto-sampling?
    AD1CON1bits.SSRC = 0b111; // auto-conversion

    AD1CON3bits.SAMC = 20; // auto-sample time = X * TAD (what is X?)
    AD1CON3bits.ADCS = 24; // clock = Y * Tcy (what is Y?)

    _AD1IF = 0; 
    _AD1IE = 1; 

    AD1CON1bits.ADON = 1;
}

int main(void) {
    setup();

    while (1);
}