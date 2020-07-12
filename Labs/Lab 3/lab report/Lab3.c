#include <p24Fxxxx.h>
#include <xc.h>

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

//Dummy Variable
#define NOKEY 255 // value of input corresponding to no key being pressed

// initialize Display Variables to be OFF - This two variables are holding the digits value
unsigned int left_digit  = 0b0000001111111000;
unsigned int right_digit = 0b0000001111111000;

// To switch between the two displays
static unsigned int digit_alternate = 0;

// Activation the pad's rows
unsigned int keyRow[4] = {  0xe000,      // row 3
                            0xd000,      // row 2
                            0xb000,      // row 1
                            0x7000       // row 0
};


//Assigning the characters to the 7 segment display
unsigned int characters[] = {
    0b0000001001111100, 0b0000000010010100, 0b0000000000110100, 0b0000000001000100,     // 1    2   3   4
    0b0000001001100100, 0b0000000100100100, 0b0000000100000100, 0b0000001100000100,     // 5    6   7   8
    0b0000000001111100, 0b0000000000000100, 0b0000000000100100, 0b0000000110001100,     // 9    10  A   B
    0b0000000111000100, 0b0000000000001100, 0b0000000110000100, 0b0000001000010100,     // C    D   E   F
};


// Shown the characters to the 7 segment display
void put(unsigned char x)
{
   left_digit  = right_digit;       // Assign the left digit to the right one
   right_digit = characters[x];     // Assign the right digit to the pressed key 
}

//Convert the number detected to a character for the 7 segment display
unsigned char padNumber[16] = {1,   2,   3, 0xA,
                               4,   5,   6, 0xB,
                               7,   8,   9, 0xC,
                               0xe, 0, 0xf, 0xD
};

// Scan rows for the key thats pressed
unsigned char scan(unsigned short int row)
{
   static unsigned char LUT[] = {NOKEY,NOKEY,NOKEY,NOKEY,NOKEY,NOKEY,NOKEY,0,NOKEY,NOKEY,NOKEY,1,NOKEY,2,3,NOKEY};
   unsigned char key;

   // Look for the column where the key was pressed 
   key = PORTA & 15;
   key = LUT[key];
   if (key == NOKEY) return key;
   
   // Match the row and the column together
   key += 4*row;
   
   // return the new key
   return  key;
}

// Find the the key thats pressed and then holds its value
void readKeyPadRAW(void)
{
   static unsigned short int row = 0;
   static unsigned char lastKey[] = {NOKEY,NOKEY,NOKEY,NOKEY};
   static unsigned short int debounce  = 0;
   unsigned char key;

   // Alternate the displays using an XOR so that the human eye sees both at the same time
   digit_alternate ^= 1;

   //1 ms timer
   while (!IFS0bits.T1IF) ;
   IFS0bits.T1IF = 0;
   
   // Wait if a key has been pressed 
   if (debounce)
   {
       debounce--;
       LATB &= 0xF003;
       return;
   }
   
   
   // If the key changes refresh the displays
   key = scan(row);
   if (key != lastKey[row])
   {
       lastKey[row] = key;
       if (key != NOKEY){
           put(key);
       }
       debounce = 40;
   }
   
   // move to the next row to check 
   row = (row+1) % 4;
   
   // clear LATB and then assign the new row
   LATB &= 0x3;
   LATB |= keyRow[row]; // sets the next row to be 

}


void initKeyPad(void){
    
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;  // For digital I/O.  If you want to use analog, you'll
    
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    
    CNPU1bits.CN2PUE = 1;
    CNPU1bits.CN3PUE = 1;
    CNPU2bits.CN30PUE = 1;
    CNPU2bits.CN29PUE = 1;
    
    // set all input pins in TRISB to output
    TRISB = 0;  

    //Set a timer of a 1 ms delay
    T1CON = 0;
    PR1 = 15999;
    TMR1 = 0;
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 1;
}


int main(void){
   
   // initialize the key pad, timer, outputs and pull_ups
   initKeyPad();
   
   while (1)
    {
       //read the value pressed form the keypad and hold it as a symbol for the display
        readKeyPadRAW();
        
        //Alternate between the displays
        if (digit_alternate)
            LATB |= 0x400 | left_digit; // The left digit
        else
            LATB |= 0x800 | right_digit; // The right digit
        
   }
   return 0; 
}


