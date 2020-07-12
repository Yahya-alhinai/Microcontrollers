#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include <p24Fxxxx.h>
#include <string.h>
#include "lcd28.h"

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


#define ON_OFF PORTAbits.RA1
#define C_F PORTAbits.RA2
#define incT PORTAbits.RA3
#define State PORTBbits.RB14 // it was RA4
#define decT PORTBbits.RB4

#define FAN PORTBbits.RB5
#define HEATER PORTBbits.RB6
#define COOLER PORTBbits.RB7

#define limit_HC 50
#define limit_LC 15
#define limit_HF 122
#define limit_LF 59
#define Safty_Shut_Down (1 * 10) //(x*10); where x is # seconds to shut down

volatile unsigned char power = 0;
volatile unsigned char convert_unit = 0;
volatile unsigned char convert_State = 0;
volatile unsigned int setTemp = 20;
volatile unsigned int ActualTemp = 0;
volatile unsigned int count_Shut_Down = 0xffff;
volatile unsigned int ADC_Buffer[1024] = { 0 };
volatile unsigned int i = 0;
volatile char adStr[20];
//volatile int StateArray[4]={"FAN","HETR","COLR","AUTO"};
void getTemp(void);

void __attribute__((__interrupt__)) _ADC1Interrupt(void)
{

    _AD1IF = 0;
    ActualTemp = (330.0 / 1023) * ADC1BUF0;
    if (convert_unit == 1) {
        convert_from_C_to_F_ActualTemp(ActualTemp);
    }
    
}
void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;
    TMR2 = 0;
}
void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    TMR1 = 0;
    getTemp();
    count_Shut_Down++;
    power = 1; // delete this line if you want it to work plz. Thanks.
    if (power == 1) {
        switch (convert_State) {
        case 0:
            if (convert_unit == 0) {
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.0f  C", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  FAN", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            else {
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.0f  F", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  FAN", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            break;
        case 1:
            if (convert_unit == 0) {
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.0f  C", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  HEAT", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            else {
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.0f  F", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  HEAT", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            break;
        case 2:
            if (convert_unit == 0) {
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.0f  C", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  COOL", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            else {
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.0f  F", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  COOL", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }

            break;
        case 3:
            if (convert_unit == 0) {
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.0f  C", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  AUTO", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            else {
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.0f  F", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  AUTO", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            break;

        default:
            lcd_cmd(0b00000001); // Clear Display
            delay1ms(2);

            lcd_setCursor(0, 0);
            lcd_printStr("OFF");
        }
    }
    else {
        lcd_cmd(0b00000001); // Clear Display
        delay1ms(2);
        lcd_setCursor(0, 0);
        lcd_printStr("OFF");
    }
}
void convert_from_C_to_F_ActualTemp(unsigned char T)
{
    ActualTemp = T * 1.8 + 32;
}
void convert_from_C_to_F(unsigned char T)
{
    setTemp = T * 1.8 + 32;
}
void convert_from_F_to_C(unsigned char T)
{
    setTemp = (T - 32) * 5 / 9;
}
void getTemp(void)
{
    asm("nop");

    if (ON_OFF == 0) {
        power ^= 1;
    }

    if (power == 1) {
        if (C_F == 0) {
            convert_unit ^= 1; // If 0, then it is Celsius, which is default. If 1, then convert to Fahrenheit

            if (convert_unit == 1) {
                convert_from_C_to_F(setTemp);
                //convert_Actual_Temp_from_C_to_F(Actual_Temp);
            }
            if (convert_unit == 0) {
                convert_from_F_to_C(setTemp);
                //convert_Actual_Temp_from_F_to_C(Actual_Temp);
            }
        }
        if (State == 0) {
            //convert_State += (convert_State & 3);
            convert_State++;
            convert_State = (convert_State & 0b11);
        }
        if (incT == 0) {
            if (convert_unit == 1) {
                if (setTemp <= limit_HF) {
                    setTemp++;
                }
            }
            if (convert_unit == 0) {
                if (setTemp <= limit_HC) {
                    setTemp++;
                }
            }
        }
        if (decT == 0) {
            if (convert_unit == 1) {
                if (setTemp > limit_LF) {
                    setTemp--;
                }
            }
            if (convert_unit == 0) {
                if (setTemp > limit_LC) {
                    setTemp--;
                }
            }
        }
    }
    else {
        convert_unit = 0;
        convert_State = 0;
        setTemp = 20;
        HEATER = 0;
        COOLER = 0;
        FAN = 0;
    }
}
void Control(void)
{
    if (power == 1) {
        switch (convert_State) {
        case 0:
            FAN = 1;
            HEATER = 0;
            COOLER = 0;
            break;
        case 1:
            if (setTemp > ActualTemp) {
                HEATER = 1;
                FAN = 1;
                COOLER = 0;
            }
            else {
                HEATER = 0;
                FAN = 0;
                COOLER = 0;
            }
            break;
        case 2:
            if (setTemp < ActualTemp) {
                COOLER = 1;
                FAN = 1;
                HEATER = 0;
            }
            else {
                COOLER = 0;
                FAN = 0;
                HEATER = 0;
            }
            break;
        case 3:
            //if (count_Shut_Down < Safty_Shut_Down) {
                //HEATER = 0;
                //COOLER = 0;
                //FAN = 0;
            //}
            //else {  //else if you remove the comments above 
                if (setTemp > ActualTemp) {
                    HEATER = 1;
                    FAN = 1;
                    COOLER = 0;
                }
                else if (setTemp > ActualTemp) {
                    COOLER = 1;
                    FAN = 1;
                    HEATER = 0;
                }
                else {
                    HEATER = 0;
                    COOLER = 0;
                    FAN = 0;
                }
                break;
            default:
                HEATER = 0;
                COOLER = 0;
                FAN = 0;
            }
        //} remove for safety debug 
            if (setTemp == ActualTemp) {
                count_Shut_Down = 0;
            }
    } 
    else {
        HEATER = 0;
        COOLER = 0;
        FAN = 0;
    }
}
void setup(void)
{
    CLKDIVbits.RCDIV = 0; //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0x9ffe;

    //SWITCHES SETUP
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA4 = 1;
    TRISBbits.TRISB4 = 1;

    CNPU1bits.CN3PUE = 1; // set pull up resistor of PORTAbits.RA1
    CNPU2bits.CN30PUE = 1; // set pull up resistor of PORTAbits.RA2
    CNPU2bits.CN29PUE = 1; // set pull up resistor of PORTAbits.RA3
    CNPU1bits.CN0PUE = 1; // set pull up resistor of PORTAbits.RA4
    CNPU1bits.CN1PUE = 1; // set pull up resistor of PORTAbits.RB4
    CNPU1bits.CN12PUE = 1; // set pull up resistor of PORTAbits.RB15

    // FAN HEATER COOLER SETUP
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    PORTBbits.RB5 = 0;
    PORTBbits.RB6 = 0;
    PORTBbits.RB7 = 0;

    //TIMER1 For the display delay of 500ms
    TMR1 = 0;
    PR1 = 32500; //500 ms delay
    _T1IF = 0;
    T1CON = 0x8030; // PRE =1:256
    IEC0bits.T1IE = 1;

    
    //TIMER2 For the display delay of 100ms
    TMR2 = 0;
    PR2 = 6500; //100 ms delay
    _T2IF = 0;
    T2CON = 0x8030; // PRE =1:256
    IEC0bits.T2IE = 1;
    
    /**************************** Temperature Sensor Setup **********************************/
    // setup Timer 3
    T3CON = 0;
    TMR3 = 0;
    PR3 = 15624; // 16 sample per second
    T3CONbits.TCKPS = 3; // pre 1:64

    // Setup on A/D
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CHS = 0;

    AD1CON1bits.ASAM = 1; // auto-sampling. P21, Sec. 17.7.3.3 in the ADC
    // document says use ASAM=1
    AD1CON1bits.SSRC = 0b010; // Use Timer3 as the conversion trigger
    // We can also use Timer3 / Timer2 as a 32-bit timer to trigger
    // A/D sampling/conversion, because the 32-bit timer
    // will also generate the T3IF event.

    //doesn't matter //AD1CON3bits.SAMC = 3; // auto-sample time = X * TAD (what is X?)
    AD1CON3bits.ADCS = 2; // clock = Y * Tcy (what is Y?)

    // Configure A/D interrupt
    _AD1IF = 0; // clear flag
    _AD1IE = 1; // enable interrupt

    AD1CON1bits.ADON = 1;

    // turn on Timer 3
    T3CONbits.TON = 1;
}

int main()
{
    setuplcd();
    lcd_init();
    setup();
    
    while (1)
        Control();
    
    return 0;
}