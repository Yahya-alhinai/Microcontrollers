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


    volatile unsigned int overflow_1 = 0;   // # of times TMR1 rolled over    
    volatile unsigned int overflow_2 = 0;   // # of times TMR1 rolled over
    
    volatile unsigned long int Ave_1 = 0;
    volatile unsigned long int Ave_2 = 0; 
    
    volatile unsigned long int On_1 = 0;
    volatile unsigned long int On_2 = 0;
    

void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;  // could have used _T2IF
    
}

void __attribute__((__interrupt__,__auto_psv__)) _T3Interrupt(void)
{
    IFS0bits.T3IF = 0;  // could have used _T3IF
    
}

void __attribute__((__interrupt__,__auto_psv__)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    overflow_1++;
    overflow_2++;
}

void __attribute__((__interrupt__,__auto_psv__)) _ISR _IC1Interrupt() 
{
    _INT0IF = 0;

    volatile static unsigned int BUFF[4] = {0, 0, 0, 0};
    volatile static unsigned long int i = 0;
    
    if (_INT0EP == 0){   // I was waiting for a rising edge
        TMR1 = 0;
        overflow_1 = 0;
    } else {
        BUFF[i % 4] = TMR1 + 16000L * overflow_1;
        i++;
        TMR1 = 0;         // uncomment these two lines if you want the period
        overflow_1 = 0;     //       and not just active duty cycle. also
                            //       delete the if part, and the last line
    }
    
    if (i == 0){
        Ave_1 = (BUFF[0]+BUFF[1]+BUFF[2]+BUFF[3])/4;
        On_2 = 1;
    }

    _INT0EP = 1 - _INT0EP;
}

void __attribute__((__interrupt__,__auto_psv__)) _ISR _IC2Interrupt() 
{
    _INT1IF = 0;

    volatile static unsigned int BUFF[4] = {0, 0, 0, 0};
    volatile static unsigned long int i = 0;
    
    if (_INT1EP == 0){   // I was waiting for a rising edge
        TMR1 = 0;
        overflow_2 = 0;
    } else {
        BUFF[i % 4] = TMR1 + 16000L * overflow_2;
        i++;
        TMR1 = 0;         // uncomment these two lines if you want the period
        overflow_2 = 0;     //       and not just active duty cycle. also
                            //       delete the if part, and the last line
    }
    
    if (i == 0){
        Ave_2 = (BUFF[0]+BUFF[1]+BUFF[2]+BUFF[3])/4;
        On_1 = 1;
    }

    _INT1EP = 1 - _INT1EP;
}


void initServo(void)
{

    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;  // For digital I/O.  If you want to use analog, you'll
    TRISB = 0;

    T3CON = 0x0010; // turn off, 1:8 pre, Tcy clock source

    PR3 = 40000;  // combined with 1:64 ==> 1,600,000, which is 100ms
    TMR3 = 0;
    _T3IF = 0;
    IEC0bits.T3IE = 1;
    
   
    OC1CON = 0;
    OC1R = 0; 
    OC1RS = 0; // We will only change this once PWM is turned on
    OC1CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC1CONbits.OCTSEL = 1; // Use Timer 3 for compare source

     __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    //RPINR7bits.IC1R = 9; // Use Pin RP9 = "9", for Input Capture 1 (Table 10-2)
    RPOR3bits.RP6R = 18; // Use Pin RP6 for Output Compare 1 = "18" (Table 10-3)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
    
    
     T3CONbits.TON = 1;
 
 }


void setServo(int Val)
{
    OC1RS = Val;   
}

void initTwoPushButtons(void)
{
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;  // For digital I/O.  If you want to use analog, you'll
    TRISA = 0x0000;
    TRISB = 0x0300;
    
    LATAbits.LATA0 = 0;
    LATAbits.LATA1 = 0;
    
    CNPU2bits.CN21PUE = 1;
    CNPU2bits.CN22PUE = 1;

    T2CON = 0x8030; // turn off, 1:256 pre, Tcy clock source
    PR2 = 0xffff;  
    TMR2 = 0;
    _T2IF = 0;
    
    
    IC1CON = 0; // Turn off and reset internal state of IC1
    IC1CONbits.ICTMR = 1; // Use Timer 2 for capture source
    IC1CONbits.ICM = 0b010; // Turn on and capture every falling edge
    IFS0bits.IC1IF = 0; //sets flag to 0
    IEC0bits.IC1IE = 1; //enables the interrupt
    
    IC2CON = 0; // Turn off and reset internal state of IC2
    IC2CONbits.ICTMR = 1; // Use Timer 2 for capture source
    IC2CONbits.ICM = 0b010; // Turn on and capture every falling edge
    IFS0bits.IC2IF = 0; //sets flag to 0
    IEC0bits.IC2IE = 1; //enables the interrupt
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR7bits.IC1R = 8; // Use Pin RP8 = "8", for Input Capture 1 (Table 10-2)
    RPINR7bits.IC2R = 9; // Use Pin RP9 = "9", for Input Capture 2 (Table 10-2)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
}

void setup(void)
{
    CLKDIVbits.RCDIV = 0; // make 16MHz
    // setup INT0 (RB7) pin as input
    AD1PCFG = 0x9fff;  // make all pins digital, not analog.
    TRISBbits.TRISB7 = 1; // make RB7 an input pin. RB7 and INT0 pin are multiplexed.

    T1CON = 0;    // turn off the timer for now, set PRE=1, TCY as input clock.
    PR1 = 15999;  // could be any number. The larger the better.
    TMR1 = 0;
    T1CONbits.TON = 1;

    IFS0bits.T1IF = 0;
    //IPC0bits.T1IP = 4;
    // enable Timer 1 interrupt. If you forget the next line, _T1Interrupt is never called
    // even if there are _T1IF events.
    IEC0bits.T1IE = 1;

    _INT0EP = 0;  // edge polarity set to detection of positive edge.
    _INT0IF = 0;  // reset so that you don't respond to a previously generated event.
    _INT0IE = 1;  // enable interrupt for INT0 events.
    
    _INT1EP = 0;  // edge polarity set to detection of positive edge.
    _INT1IF = 0;  // reset so that you don't respond to a previously generated event.
    _INT1IE = 1;  // enable interrupt for INT0 events.

}


int main(void) 
{
    setup();
    initServo();
    initTwoPushButtons();
    
    while(1){
        if (On_1 && On_2){
            if (Ave_1 > Ave_2){
                _LATA0 = 0;
            }
            else{ 
                _LATA0 = 1;
            }
        On_1 = 0;
        On_2 = 0;
        }
        
        
    };
    
    return 0;
}