
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

//Global Variables
#define NOKEY 255 // value of input corresponding to no key being pressed

// initialize Display Variables to be Zero  
unsigned int digit[2] = {0b0000001111111100, 0b0000001111111100}; // holds display encoding of values to be displayed

static unsigned int flip = 0;

// KBMASK1 is used to mask off the bits in PORTB we don't use (RB12-RB0)
#define KBMASK1 0x0fff

unsigned int keyRowEnPattern[4] = {0xe000, 0xd000, 0xb000, 0x7000};


unsigned int patterns[] = {
   0b0000000000001100,   // '0'
   0b0000001001111100,   // '1'
   0b0000000010010100,   // '2'
   0b0000000000110100,   // '3'
   0b0000001001100100,   // '4'
   0b0000000100100100,   // '5'
   0b0000000100000100,   // '6'
   0b0000000001111100,   // '7'
   0b0000000000000100,   // '8'
   0b0000000000100100,   // '9'
   0b0000000001000100,   // 'A'
   0b0000001100000100,   // 'b'
   0b0000000110001100,   // 'C'
   0b0000001000010100,   // 'd'
   0b0000000110000100,   // 'E'
   0b0000000111000100,   // 'F'
};


void put(unsigned char x)
{
   digit[0] = digit[1]; // sets the left digit
   digit[1] = patterns[x]; // sets the right digit
}

unsigned char lookupTable[16] = {1, 2, 3, 0xA, 4, 5, 6, 0xB, 7,8,9, 0xC, 0xe, 0, 0xf, 0xD };                // around the elements to make things turn out like you want.
// Scan 
unsigned char scan(unsigned short int row)
{
   static unsigned char LUT[] = {NOKEY,NOKEY,NOKEY,NOKEY,NOKEY,NOKEY,NOKEY,0,NOKEY,NOKEY,NOKEY,1,NOKEY,2,3,NOKEY};
   unsigned char key;

   key = PORTA & 15;

   if (key == NOKEY) return key;
   key += 4*row;
   key = lookupTable[key];

   return  key;
}

void waitOneMillisecondAndCheckKeypad(void)
{
   static unsigned short int row = 0;
   static unsigned char lastKey[] = {NOKEY,NOKEY,NOKEY,NOKEY};
   static unsigned short int debounce  = 0;
   unsigned char key;

   while (!IFS0bits.T1IF) ;
   IFS0bits.T1IF = 0;
   if (debounce)
   {
       debounce--;
       return;
   }
   key = scan(row);
   if (key != lastKey[row])
   {
       lastKey[row] = key;
       if (key != NOKEY)
           put(key);
       
           
       debounce = 40;
   }
   row++; // move to next row
   row &= 3;
   LATB = LATB & KBMASK1;
   LATB |= keyRowEnPattern[row]; // sets the next row to be scanned
}


void initKeyPad(void){
    
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;  // For digital I/O.  If you want to use analog, you'll
                       // need to change this.
    
    // add your configuration commands below
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    
    CNPU1bits.CN2PUE = 1;
    CNPU1bits.CN3PUE = 1;
    CNPU2bits.CN30PUE = 1;
    CNPU2bits.CN29PUE = 1;

    // note a different method used to set RB12..15 to 1
    // compared to what we did with TRISA0..3. Which one
    // do you like better?
    TRISB &= 0;  // set TRISB12..15 to 0 (output)

    // if you haven't covered timers in your lecture, the following 5 lines are
    // gibberish. They setup a "timer" to give us 1ms delay.
    T1CON = 0;
    PR1 = 15999;
    TMR1 = 0;
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 1;
}


int main(void){
    
   initKeyPad();
   
   while (1)
    {
        waitOneMillisecondAndCheckKeypad();
        
        LATB &= 0b1111000000000011;

        if (flip)
            LATB |= digit[0] | 0b0000010000000000; // The left digit
        else
            LATB |= digit[1] | 0b0000100000000000; // The right digit

        flip ^= 1;
   }
   
   return 0; 
}
