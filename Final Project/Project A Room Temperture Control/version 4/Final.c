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
volatile int ADC_Buffer[1024];
volatile int i = 0;
volatile char adStr[20];
volatile int update = 0;


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
        lcd_cmd(1);
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
                    if (update)
                        Send_write(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  FAN", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update){
                        Send_write(adStr);
                        update = 0;
                    }
                } else{                    
                    lcd_setCursor(0, 0);
                    if (setTemp < 100)
                        sprintf(adStr, "%2.0f  F", setTemp); // ?x.xxxx V?
                    else
                        sprintf(adStr, "%2.0f F", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update)
                        Send_write(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  FAN", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update){
                        Send_write(adStr);
                        update = 0;
                    }
                }
                break;
            case 1:
                if (convert_unit == 0) {
                    lcd_setCursor(0, 0);
                    sprintf(adStr, "%2.0f  C", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update)
                        Send_write(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  HEAT", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update){
                        Send_write(adStr);
                        update = 0;
                    }
                } else {
                    lcd_setCursor(0, 0);
                    if (setTemp < 100)
                        sprintf(adStr, "%2.0f  F", setTemp); // ?x.xxxx V?
                    else
                        sprintf(adStr, "%2.0f F", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update)
                        Send_write(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  HEAT", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update){
                        Send_write(adStr);
                        update = 0;
                    }
                }
                break;
            case 2:
                if (convert_unit == 0) {
                    lcd_setCursor(0, 0);
                    sprintf(adStr, "%2.0f  C", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr);
                    if (update)
                        Send_write(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  COOL", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update){
                        Send_write(adStr);
                        update = 0;
                    }
                } else {
                    lcd_setCursor(0, 0);
                    if (setTemp < 100)
                        sprintf(adStr, "%2.0f  F", setTemp); // ?x.xxxx V?
                    else
                        sprintf(adStr, "%2.0f F", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); 
                    if (update)
                        Send_write(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  COOL", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); 
                    if (update){
                        Send_write(adStr);
                        update = 0;
                    }
                }

                break;
            case 3:
                if (convert_unit == 0) {
                    lcd_setCursor(0, 0);
                    sprintf(adStr, "%2.0f  C", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update)
                        Send_write(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  AUTO", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update){
                        Send_write(adStr);
                        update = 0;
                    }
                } else {
                    lcd_setCursor(0, 0);
                    if (setTemp < 100)
                        sprintf(adStr, "%2.0f  F", setTemp); // ?x.xxxx V?
                    else
                        sprintf(adStr, "%2.0f F", setTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update)
                        Send_write(adStr);
                    
                    lcd_setCursor(1, 0);
                    sprintf(adStr, "%2.0f  AUTO", ActualTemp); // ?x.xxxx V?
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    if (update){
                        Send_write(adStr);
                        update = 0;
                    }
                }
                break;
        }
    } 
}


void getTemp(void)
{
    // Turn ON-OFF
    if (!ON_OFF){
        power ^= 1;
        lcd_cmd(1);
    }
    
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

//---------BLUETOOTH------------//
volatile unsigned char Buff[1000];
volatile unsigned char front = 0;
volatile unsigned char rear = 0;

void __attribute__((__interrupt__,__auto_psv__)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;
    Buff[front++] = U1RXREG;
    front &= 1000-1;
}


unsigned char Get_Char(void)
{
    unsigned char x;

    if (front != rear){
        x = Buff[rear++];
        rear &= 1000-1;
        return x;
    }
    return 0;
}

void Send_write(char word[]){
    int i;
    
    for (i = 0; i < strlen(word); i++){
        while (!IFS0bits.U1TXIF);
        IFS0bits.U1TXIF = 0;
        U1TXREG = word[i];
    }
    Send_Char(0x0D);  // new line characters
    Send_Char(0x0A);
}

void Send_Char(const unsigned char Char)
{
    
    while (!IFS0bits.U1TXIF);
    IFS0bits.U1TXIF = 0;
    U1TXREG = Char;
    
    
    if(Char == '1'){
        if (convert_unit && setTemp < LIMIT_H_F){
            setTemp++;
        }
        if (!convert_unit && setTemp < LIMIT_H_C)
            setTemp++;
        
        Send_write(": Temperature increment");
        update = 1;
    }
    
    if(Char == '2'){
        if (convert_unit && setTemp > LIMIT_L_F)
            setTemp--;

        if (!convert_unit && setTemp > LIMIT_L_C) 
            setTemp--;
        
        Send_write(": Temperature decrement");
        update = 1;
    }
    
    if(Char == '3'){
        convert_unit ^= 1; // If 0, then it is Celsius, which is default. If 1, then convert to Fahrenheit
        
        if (convert_unit){
            convert_from_C_to_F(setTemp);
            convert_from_C_to_F_ActualTemp(ActualTemp);
            Send_write(": Switch to fahrenheit");
        }
        else{
            convert_from_F_to_C(setTemp);
            ActualTemp = (330.0 / 1023) * ADC1BUF0;
            Send_write(": Switch to celsius");
        }
        update = 1;
    }
    
    
    if(Char == '5'){
        convert_STATE = 0;
        Send_write(": Switch to FAN state");
        update = 1;
    }
    
    if(Char == '6'){
        convert_STATE = 1;
        Send_write(": Switch to HEAT state");
        update = 1;
    }
    
    if(Char == '7'){
        convert_STATE = 2;
        Send_write(": Switch to COOL state");
        update = 1;
    }
    
    if(Char == '8'){
        convert_STATE = 3;
        Send_write(": Switch to AUTO state");
        update = 1;
    }
    
    
    if (Char == '0'){
        power ^= 1;
        lcd_cmd(1);
        if (power)
            Send_write(": The Power is OFF");
        else
            Send_write(": The Power is ON");
        update = 1;
    }
        
}


//---------------------------------------//


int main()
{
    setup();
    
    int i;
    for (i = 0; i < 40; i++){
        Send_Char(0x0D);
        Send_Char(0x0A);
    }
    
    
    Send_write("1: Increment temperature");
    Send_write("2: Decrement temperature");
    Send_write("3: Change temperature unit");
    
    Send_write("5: SWITCH TO FAN STATE");
    Send_write("6: SWITCH TO HEAT STATE");
    Send_write("7: SWITCH TO COOL STATE");
    Send_write("8: SWITCH TO AUTO STATE");

    Send_write("0: Turn ON/OFF");
    
    
    while (1){
        Control();        
        Send_Char(Get_Char());
    }
    
    return 0;
}