/*
 * File:   Lab2B_C.c
 * Author: yahya
 *
 * Created on February 3, 2018, 10:51 AM
 */


#include "xc.h"
#include "Lab2B_ASM.h"
#include "stdint.h"

#define FRAME_PER 1


// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1 // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF // JTAG Port Enable (JTAG port is disabled)
// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME // Clock Switching and Monitor (Clock switching is enabled,
 // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL)

int holder;

void writeColor(int r, int g, int b){
    wait_50us();
    
    LATB = r;
    
    if (LATBbits.LATB7)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB6)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB5)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB4)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB3)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB2)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB1)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB0)
        write_1();
    else
        write_0();
    
    
    LATB = g;
    
    if (LATBbits.LATB7)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB6)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB5)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB4)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB3)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB2)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB1)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB0)
        write_1();
    else
        write_0();
    
    
            
    LATB = b;
    
    if (LATBbits.LATB7)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB6)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB5)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB4)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB3)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB2)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB1)
        write_1();
    else
        write_0();
    
    if (LATBbits.LATB0)
        write_1();
    else
        write_0();

};



void drawFrame (unsigned char frame){
    
    while(frame--){
        writeColor(frame, 0, 255-frame);
        delay(FRAME_PER);
    }
            
    frame = 255;
    while(frame--){
        writeColor(255-frame, 0, frame);
        delay(FRAME_PER);
        
    }
   
}

void delay(int delay_in_ms){
    while(delay_in_ms--)
        wait_1ms();
    
}



//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x


uint32_t packColor(unsigned char Red, unsigned char Blu, unsigned char Grn){
    unsigned long int RGBval = 0;
    RGBval = ((long) Red << 16) | ((long) Grn << 8) | ((long) Blu);
    return RGBval;
}

unsigned char getR(uint32_t RGBval){
    unsigned char Red = 0;
    Red = (unsigned char) (RGBval >> 16);
    return Red;
}

unsigned char getG(uint32_t RGBval){
    unsigned char Green = 0;
    Green = (unsigned char) (RGBval >> 8 );
    return Green; 
}

unsigned char getB(uint32_t RGBval){
    unsigned char Blue = 0;
    Blue = (unsigned char) (RGBval >> 0 );
    return Blue;
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.

void Wheel_16(unsigned char WheelPos){
    delay(FRAME_PER);
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85)
    {
        return writeColor(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else if(WheelPos < 170)
    {
        WheelPos -= 85;
        return writeColor(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return writeColor(WheelPos * 3, 255 - WheelPos * 3, 0);
    }

}


//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
//x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x
void setup(void){
    CLKDIVbits.RCDIV = 0; //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0x9fff;
    TRISA = 0xFFFE;
    LATA = 0x0000;        
}

int main(void)
{
    setup();
    while(1) {
        // Execute repeatedly forever and ever and ever and ever ...
        asm("nop");
        
        //writeColor(1, 1, 0);
        //writeColor(0, 128, 128);
        
        
        //drawFrame(255);
        
        int i = 255;
        while(i--){
            Wheel_16(i);
            
        }

        asm("nop");
    }
    return 0;
}