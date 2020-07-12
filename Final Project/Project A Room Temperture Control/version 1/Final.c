#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd28.h"


#define ON_OFF PORTAbits.RA1
#define C_F PORTAbits.RA2
#define INC_T PORTAbits.RA3
#define DEC_T PORTBbits.RB4
#define STATE PORTBbits.RB14 // it was RA4

#define FAN PORTBbits.RB5
#define HEATER PORTBbits.RB6
#define COOLER PORTBbits.RB7

#define LIMIT_H_C 50
#define LIMIT_L_C 15
#define LIMIT_H_F 122
#define LIMIT_L_F 59
#define SHUT_DOWN (1 * 10) //(x*10); where x is # seconds to shut down

volatile char power = 0;
volatile char convert_unit = 0;
volatile char convert_STATE = 0;
volatile int setTemp = 20;
volatile int ActualTemp = 0;
volatile int count_Shut_Down = 0xffff;
volatile int ADC_Buffer[1024] = { 0 };
volatile int i = 0;
volatile char adStr[20];


void getTemp(void);

void convert_from_C_to_F_ActualTemp(unsigned char T){
    ActualTemp = T * 1.8 + 32;
}

void convert_from_C_to_F(unsigned char T){
    setTemp = T * 1.8 + 32;
}

void convert_from_F_to_C(unsigned char T){
    setTemp = (T - 32) * 5 / 9;
}

void __attribute__((__interrupt__,__auto_psv__)) _ADC1Interrupt(void)
{
    _AD1IF = 0;
    ActualTemp = (330.0 / 1023) * ADC1BUF0;
    if (convert_unit == 1) {
        convert_from_C_to_F_ActualTemp(ActualTemp);
    }
    
}

void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    TMR1 = 0;
    getTemp();
    count_Shut_Down++;
    power = 1; // delete this line if you want it to work plz. Thanks.
    if (power == 1) {
        switch (convert_STATE) {
        case 0:
            if (convert_unit == 0) {
                lcd_cmd(0b00000001); // Clear Display
                wait(2);
                sprintf(adStr, "%2.0f  C", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  FAN", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            else {
                lcd_cmd(0b00000001); // Clear Display
                wait(2);
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
                wait(2);
                sprintf(adStr, "%2.0f  C", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  HEAT", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            else {
                lcd_cmd(0b00000001); // Clear Display
                wait(2);
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
                wait(2);
                sprintf(adStr, "%2.0f  C", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  COOL", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            else {
                lcd_cmd(0b00000001); // Clear Display
                wait(2);
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
                wait(2);
                sprintf(adStr, "%2.0f  C", (1.0) * setTemp); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.0f  AUTO", (1.0) * ActualTemp); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
            }
            else {
                lcd_cmd(0b00000001); // Clear Display
                wait(2);
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
            wait(2);

            lcd_setCursor(0, 0);
            lcd_printStr("OFF");
        }
    }
    else {
        lcd_cmd(0b00000001); // Clear Display
        wait(2);
        lcd_setCursor(0, 0);
        lcd_printStr("OFF");
    }
}


void getTemp(void)
{
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
        if (STATE == 0) {
            //convert_STATE += (convert_STATE & 3);
            convert_STATE++;
            convert_STATE = (convert_STATE & 0b11);
        }
        if (INC_T == 0) {
            if (convert_unit == 1) {
                if (setTemp <= LIMIT_H_F) {
                    setTemp++;
                }
            }
            if (convert_unit == 0) {
                if (setTemp <= LIMIT_H_C) {
                    setTemp++;
                }
            }
        }
        if (DEC_T == 0) {
            if (convert_unit == 1) {
                if (setTemp > LIMIT_L_F) {
                    setTemp--;
                }
            }
            if (convert_unit == 0) {
                if (setTemp > LIMIT_L_C) {
                    setTemp--;
                }
            }
        }
    }
    else {
        convert_unit = 0;
        convert_STATE = 0;
        setTemp = 20;
        HEATER = 0;
        COOLER = 0;
        FAN = 0;
    }
}

void Control(void) {
    if (power == 1) {
        switch (convert_STATE) {
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
                } else {
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
                } else {
                    COOLER = 0;
                    FAN = 0;
                    HEATER = 0;
                }
                break;
            case 3:
                if (setTemp > ActualTemp) {
                    HEATER = 1;
                    FAN = 1;
                    COOLER = 0;
                } else if (setTemp > ActualTemp) {
                    COOLER = 1;
                    FAN = 1;
                    HEATER = 0;
                } else {
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
        
        if (setTemp == ActualTemp) {
            count_Shut_Down = 0;
        }
    } else{
        HEATER = 0;
        COOLER = 0;
        FAN = 0;
  }
}
void setup(void)
{
    CLKDIVbits.RCDIV = 0; //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0x9ffe;

    //------------PINS SETUP------------//
    _TRISA1 = 1;
    _TRISA2 = 1;
    _TRISA3 = 1;
    _TRISA4 = 1;
    _TRISB4 = 1;

    _CN3PUE  = 1;   // RA1 pull up
    _CN30PUE = 1;  // RA2 pull up
    _CN29PUE = 1;  // RA3 pull up
    _CN0PUE  = 1;   // RA4 pull up
    _CN1PUE  = 1;   // RB4 pull up
    _CN12PUE = 1;  //RA15 pull up

    // FAN HEATER COOLER SETUP
    _TRISB5 = 0;
    _TRISB6 = 0;
    _TRISB7 = 0;
    _LATB5 = 0;
    _LATB6 = 0;
    _LATB7 = 0;
    //----------------------------------//
    
    
    //------------LCD SETUP------------//
    I2C2CON = 0;
    I2C2BRG = 157;
    I2C2CONbits.I2CEN = 1;
    _MI2C2IF = 0;
    //---------------------------------//
    
    
    //------SCREEN REFRESH RATE (TMR1)--------//
    TMR1 = 0;
    PR1 = 18750; //0.3 s 
    _T1IF = 0;
    T1CON = 0x8030;
    IEC0bits.T1IE = 1;
    //---------------------------------//

    
    //-----------SAMPLING -------------//
    //SAMPLING RATE
    T3CON = 0;
    TMR3 = 0;
    PR3 = 18750;
    T3CON = 0x8030;

    // Setup on A/D
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CHS = 0;

    AD1CON1bits.ASAM = 1;
    AD1CON1bits.SSRC = 0b010;
    AD1CON3bits.ADCS = 2;

    _AD1IF = 0;
    _AD1IE = 1;

    AD1CON1bits.ADON = 1;
    //---------------------------------//
    
}


int main()
{
    setup();
    lcd_init();
    
    while (1)
        Control();
    
    return 0;
}