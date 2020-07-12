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


volatile unsigned char buffer[64];
volatile unsigned char front = 0;
volatile unsigned char back = 0;


void PPutch(const unsigned char c)
{
    while (!IFS0bits.U1TXIF) ;
    IFS0bits.U1TXIF = 0;
    U1TXREG = c;
}

unsigned char PGetch(void)
{
    unsigned char x;

    while (front == back) ;
    x = buffer[back++];
    back &= 63;
    return x;
}

void setup(void)
{
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;  // For digital I/O.

    _TRISB5 = 0;  // U1TX output

    U1MODE = 0;  // UEN <1:0> bits control the pins
    U1MODEbits.BRGH = 1;
    U1BRG = 34; // 115000 baud 
    U1MODEbits.UEN = 0;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    
    U1MODEbits.PDSEL = 0b10;    // Odd parity - 9 bits data
    U1MODEbits.STSEL = 1;       // Two stop bits
    //Total = 9 + 1 + 2 = 12
    
    // Peripheral Pin Select 
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock
    _RP5R = 0x0003;   //RB6 -> UART1:U1TX; See Table 10-3 on P109 of the datasheet
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock

}

int main(int argc, char *argv[])
{
    setup();
    int i;
    
    // waste some time
    for (i = 0; i < 30000; i++) i = i;

    // transmit data on UART
    for (i = 'a'; i <= 'g' ; i++)
        PPutch(i);

    
    
    while (1);
    
    
    return 0;
}