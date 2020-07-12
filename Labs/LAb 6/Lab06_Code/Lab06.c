#include <p24Fxxxx.h>
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Lab06_header.h"


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



#define SAMPLES 32
#define Rate_OF_CHANGE 2

//Storing the samles
volatile int B[1024];
//collecting sameles from ADC! BUF0
volatile int adValue = 0;

//storing the string voltage and the standard deviation of the voltage values
char adStr[20];
char adStr2[20];

//keep track of the index
volatile int idx = 0;

//storing the numerical voltage and the standard deviation of the voltage values 
volatile double Ave = 0;
volatile double Dev = 0;

//Changeing values ever Rate_OF_CHANGE times
volatile int start = 0;


void __attribute__((__interrupt__,__auto_psv__)) _ADC1Interrupt(void) 
{
    _AD1IF = 0;
    adValue = ADC1BUF0;
    
    // circulate storing value every 10 second == 1024 samples
    B[idx++] = adValue;
    idx %= 1024; 
    
}


void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt(void)
{
    IFS0bits.T2IF= 0;
    int j;
    int index;
    
    // Enter the loop to change values once you reach Rate_OF_CHANGE samples
    if (start++ % Rate_OF_CHANGE == 0){
        Ave = 0;
        
        //Sum up the last SAMPLES value
        for (j = 0; j < SAMPLES; j++){
            index = idx-1-j;
            if (index < 0) index += 1024;
            
            Ave += B[index];
        }
        Ave /= SAMPLES;
    }
}

int main(void) {
    
    setup();   
    lcd_init();    
    
    while(1){
        
        //The voltage value is bing printed 
        lcd_setCursor(0,0);
        sprintf(adStr, "%4.3f", (3.3/(1023))*Ave);
        lcd_printStr(adStr);
        
        lcd_setCursor(1,0);
        sprintf(adStr, "mA");
        lcd_printStr(adStr);
    }
   
    return 0;

}