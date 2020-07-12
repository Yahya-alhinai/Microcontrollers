#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Final_header.h"


volatile char power = 0;
volatile char convert_unit = 0;
volatile char convert_STATE = 0;
volatile float setTemp = 20;
volatile float ActualTemp = 0;
//volatile int count_Shut_Down = 0;
volatile int ADC_Buffer[1024];
volatile int i = 0;
volatile char adStr[20];

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
    if (convert_unit == 1)
        convert_from_C_to_F_ActualTemp(ActualTemp);
    
}

void __attribute__((__interrupt__, __auto_psv__)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    
    if (power) {
        lcd_setCursor(0, 0);
        lcd_printStr("OFF");
    } else{
        getTemp();
        lcd_cmd(1);
        switch (convert_STATE) {
            case 0:
                if (convert_unit == 0) {
                    lcd_setCursor(0, 0);
                    sprintf(adStr, "%2.0f  C", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  FAN", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                } else{                    
                    lcd_setCursor(0, 0);
                    if (setTemp < 100)
                        sprintf(adStr, "%2.0f  F", setTemp); // ?x.xxxx V?
                    else
                        sprintf(adStr, "%2.0f F", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  FAN", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                }
                break;
            case 1:
                if (convert_unit == 0) {
                    lcd_setCursor(0, 0);
                    sprintf(adStr, "%2.0f  C", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  HEAT", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                } else {
                    lcd_setCursor(0, 0);
                    if (setTemp < 100)
                        sprintf(adStr, "%2.0f  F", setTemp); // ?x.xxxx V?
                    else
                        sprintf(adStr, "%2.0f F", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  HEAT", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                }
                break;
            case 2:
                if (convert_unit == 0) {
                    lcd_setCursor(0, 0);
                    sprintf(adStr, "%2.0f  C", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  COOL", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                } else {
                    lcd_setCursor(0, 0);
                    if (setTemp < 100)
                        sprintf(adStr, "%2.0f  F", setTemp); // ?x.xxxx V?
                    else
                        sprintf(adStr, "%2.0f F", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  COOL", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                }

                break;
            case 3:
                if (convert_unit == 0) {
                    lcd_setCursor(0, 0);
                    sprintf(adStr, "%2.0f  C", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  AUTO", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                } else {
                    lcd_setCursor(0, 0);
                    if (setTemp < 100)
                        sprintf(adStr, "%2.0f  F", setTemp); // ?x.xxxx V?
                    else
                        sprintf(adStr, "%2.0f F", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  AUTO", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                }
                break;
        }
    } 
}


void getTemp(void)
{
    // Turn ON-OFF
    if (!ON_OFF)
        power ^= 1;
    
    if (power) {
        convert_unit = 0;
        convert_STATE = 0;
        setTemp = 20;
        HEATER = 0;
        COOLER = 0;
        FAN = 0;
    } else{
        // Convert units
        if (!C_F) {
            convert_unit ^= 1; // If 0, then it is Celsius, which is default. If 1, then convert to Fahrenheit
            if (convert_unit)
                convert_from_C_to_F(setTemp);
            else
                convert_from_F_to_C(setTemp);  
        }
        
        // Next state
        if (!STATE)
            convert_STATE = ++convert_STATE & 0b11;
        
        // Increase temp
        if (!INC_T) {
            if (convert_unit && setTemp < LIMIT_H_F)
                setTemp++;
                    
            if (!convert_unit && setTemp < LIMIT_H_C)
                setTemp++;
        }
        
        // Decrease temp
        if (!DEC_T) {
            if (convert_unit && setTemp > LIMIT_L_F)
                setTemp--;

            if (!convert_unit && setTemp > LIMIT_L_C) 
                setTemp--;
        }
    }
}

void Control(void) {
    if (power) {
        HEATER = 0;
        COOLER = 0;
        FAN = 0;
    } else{
        switch (convert_STATE) {
            case 0: // FAN
                FAN = 1;
                HEATER = 0;
                COOLER = 0;
                break;
            case 1: // HEAT + FAN
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
            case 2: // COOL + FAN
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
            case 3: // AUTO
                if (setTemp > ActualTemp) {
                    HEATER = 1;
                    FAN = 1;
                    COOLER = 0;
                } else if (setTemp < ActualTemp) {
                    COOLER = 1;
                    FAN = 1;
                    HEATER = 0;
                } else {
                    COOLER = 0;
                    FAN = 0;
                    HEATER = 0;
                }
                break;
        }
    }
}

int main()
{
    setup();
    lcd_init();
    
    while (1){
        Control();  
    }
    
    return 0;
}