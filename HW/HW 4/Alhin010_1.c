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



volatile unsigned long int cycles = 0;  
volatile unsigned long int cycles2 = 0;  
volatile unsigned long int last = 0;  

void __attribute__((__interrupt__,__auto_psv__)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
}


void __attribute__ ((__interrupt__,__auto_psv__)) _IC1Interrupt(void)
{
    cycles = TMR1; 
    TMR1 = 0;

    IFS0bits.IC1IF = 0; // Reset respective interrupt flag
    
    IC2CONbits.ICM = 0b011;

    if (cycles != last){
        PR3 = cycles >> 3;
        OC1RS = cycles >> 4;
        last = cycles;
    }
}


void __attribute__ ((__interrupt__,__auto_psv__)) _IC2Interrupt(void)
{
    cycles2 = TMR1;
    IFS0bits.IC2IF = 0; // Reset respective interrupt flag    
    IC2CONbits.ICM = 0;
    
}

void setup(void)
{
    
    //-----------SETUP------------//

    CLKDIVbits.RCDIV = 0; // make 16MHz
    AD1PCFG = 0x9fff;  // make all pins digital, not analog.
    
    TRISBbits.TRISB12 = 1; // make RB12 an input pin. 
    TRISBbits.TRISB15 = 0; // make RB15 an output pin.

    //----------------------------//

    
    //---------TIMER CLOCK---------//
    T1CON = 0;    // turn off the timer for now, set PRE=1, TCY as input clock.
    PR1 = 0xFFFF;  // could be any number. The larger the better.
    TMR1 = 0;
    T1CONbits.TON = 1;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    
    //-----------------------------//
    
    
    //------COUTPUT COMPARATOR-----//
    
    T3CON = 0; // turn off
    PR3 = 2000;
    TMR3 = 0;
    _T3IF = 0;

    OC1CON = 0;
    OC1R = 1000; 
    OC1RS = 1000; // We will only change this once PWM is turned on
    OC1CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC1CONbits.OCTSEL = 1; // Use Timer 3 for compare source
    
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPOR7bits.RP15R = 18; // Use Pin RP6 for Output Compare 1 = "18" (Table 10-3)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
    
     T3CONbits.TON = 1;
    
    //------------------------//

    
    //------INPUT CAPTURE-----//
    
    TMR2 = 0;
    T2CON = 0;
    PR2 = 0xFFFF;
    T2CONbits.TON = 1;
    TMR2 = 0;
    
    IC1CON = 0; // Turn off and reset internal state of IC1
    IC1CONbits.ICTMR = 1; // Use Timer 2 for capture source
    IC1CONbits.ICM = 0b011; //  Turn on and capture every rasing edge
    IFS0bits.IC1IF = 0; //sets flag to 0
    IEC0bits.IC1IE = 1; //enables the interrupt
    IPC0bits.IC1IP = 2; // Set module interrupt priority as 1

    
    IC2CON = 0; // Turn off and reset internal state of IC1
    IC2CONbits.ICTMR = 0; // Use Timer 2 for capture source
    IC2CONbits.ICM = 0;  // 0b011; //  Turn on and capture every rasing edge
    IFS0bits.IC2IF = 0; //sets flag to 0
    IEC0bits.IC2IE = 1; //enables the interrupt
    
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR7bits.IC1R = 12; // Use Pin RP12 = "12" for Input Capture 1 (Table 10-2)  
    RPINR7bits.IC2R = 15; // Use Pin RP12 = "12" for Input Capture 1 (Table 10-2)    
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    
    //------------------------//
}


int main(void)
{
    setup();
    
    while (1);
    
    return 0;
}
