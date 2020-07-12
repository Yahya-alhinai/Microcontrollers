/*
 * File:   PID_POWER.c
 * Author: dali9
 *
 * Created on April 29, 2018, 8:04 PM
 */
#include <p24Fxxxx.h>
#include "xc.h"
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd28.h"


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
#pragma config FNOSC = FRCPLL // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

#define ON_OFF  PORTBbits.RB4
#define outputX PORTAbits.RA4
#define decV    PORTAbits.RA3
#define incV    PORTAbits.RA2
#define limit_HV 10
#define limit_LV 0

#define KP 0.5
#define KI 0.5
#define KD 0.5

volatile unsigned char power = 0;
volatile unsigned char output_x = 0;
volatile float v_set[5]={0};
volatile float v_read[5]={0};
volatile float V_SET[5]={0};
volatile float PID[5]={0};
volatile float E1[5]={0};
volatile float E0[5]={0};
volatile double T_sampling= 1/14222;
volatile char adStr[20];

void __attribute__((__interrupt__,__auto_psv__)) _ADC1Interrupt(void)
{
    IFS0bits.AD1IF = 0;
    E0[0]=E1[0];
    v_read[0]=ADC1BUF0;
    E1[0] = ADC1BUF0*(3.33/1024)-V_SET[0];
    E0[1]=E1[1];
    v_read[1]=ADC1BUF1;
    E1[1] = ADC1BUF1*(3.33/1024)-V_SET[1];
    E0[2]=E1[2];
    v_read[2]=ADC1BUF2;
    E1[2] = ADC1BUF2*(3.33/1024)-V_SET[2];
    E0[3]=E1[3];
    v_read[3]=ADC1BUF3;
    E1[3] = ADC1BUF3*(3.33/1024)-V_SET[3];
    E0[4]=E1[4];
    v_read[4]=ADC1BUF4;
    E1[4] = ADC1BUF4*(3.33/1024)-V_SET[4];
}
void __attribute__((__interrupt__,__auto_psv__)) _T1Interrupt(void)
{
   IFS0bits.T1IF=0;
   TMR1=0;
   Display();
   user_volatge(); 
}   
void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt(void)
{
   IFS0bits.T2IF=0;
   TMR2=0;
   
   OC1RS += KP*E1[0]+KI*T_sampling*E1[0]+KD*((E1[0]-E0[0])/T_sampling);
   if(OC1RS<0)
        OC1RS=0;
   OC2RS += KP*E1[1]+KI*T_sampling*E1[1]+KD*((E1[1]-E0[1])/T_sampling);
   if(OC2RS<0)
        OC2RS=0;
   OC3RS += KP*E1[2]+KI*T_sampling*E1[2]+KD*((E1[2]-E0[2])/T_sampling);
   if(OC3RS<0)
        OC3RS=0;
   OC4RS += KP*E1[3]+KI*T_sampling*E1[3]+KD*((E1[3]-E0[3])/T_sampling);
   if(OC4RS<0)
        OC4RS=0;
   OC5RS += KP*E1[4]+KI*T_sampling*E1[4]+KD*((E1[4]-E0[4])/T_sampling);
   if(OC5RS<0)
        OC5RS=0;
}

void setup_OCRX(void){
    CLKDIVbits.RCDIV = 0; //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0x9ffe;
    
    TRISBbits.TRISB15=0;
    TRISBbits.TRISB14=0;
    TRISBbits.TRISB13=0;
    TRISBbits.TRISB12=0;
    TRISBbits.TRISB11=0;
    
    T2CON = 0x0000; // turn off,
    PR2 = 1124;  // combined with 1:64 ==> 1,600,000, which is 100ms
    TMR2 = 0;
    _T2IF = 0;
    
    
    OC1CON = 0;
    OC1R = 1000; // Make it equal 
    OC1RS = 1000; // We will only change this once PWM is turned on
    OC1CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC1CONbits.OCTSEL = 0; // Timer2=0//Timer3=1//
    
    OC2CON = 0;
    OC2R = 100; // Make it equal 
    OC2RS = 2000; // We will only change this once PWM is turned on
    OC2CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC2CONbits.OCTSEL = 0; // Timer2=0//Timer3=1//
    
    OC3CON = 0;
    OC3R = 100; // Make it equal 
    OC3RS = 3000; // We will only change this once PWM is turned on
    OC3CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC3CONbits.OCTSEL = 0; // Timer2=0//Timer3=1//
    
    OC4CON = 0;
    OC4R = 10; // Make it equal 
    OC4RS = 4000; // We will only change this once PWM is turned on
    OC4CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC4CONbits.OCTSEL = 0; // Timer2=0//Timer3=1//
    
    OC5CON = 0;
    OC5R = 10; // Make it equal 
    OC5RS = 4500; // We will only change this once PWM is turned on
    OC5CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC5CONbits.OCTSEL = 0; // Timer2=0//Timer3=1//
    
    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPOR7bits.RP15R =18; //use pin RP15 for output compare 1="18"
    RPOR7bits.RP14R =19; //use pin RP14 for output compare 2="19"
    RPOR6bits.RP13R =20; //use pin RP13 for output compare 3="20"
    RPOR6bits.RP12R =21; //use pin RP12 for output compare 4="21"
    RPOR5bits.RP11R =22; //use Pin RP11 for Output Compare 5="22" 
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS   
    
    IEC0bits.T2IE = 1;
    T2CONbits.TON = 1;// Turn on OC
}
void setup_adc_channels(void){
    AD1CON1 = 0;    // use default settings, AND turn off A/D for now
    AD1CON1bits.SSRC = 0b010;  // timer 3 rollover stops sampling, starts conversion
    AD1CON1bits.ASAM = 1; // auto-sample

    AD1CON2 = 0;
    AD1CON2bits.CSCNA = 1; // turn on scan
    AD1CON2bits.SMPI = 0b100; // interrupt after 5 conversions
    AD1CON2bits.BUFM = 1;  // two 8-word buffers

    AD1CON3bits.ADCS = 1; // Tad = 2*Tcy = 125ns
    AD1CON3bits.ADRC = 0;
    AD1CON3bits.SAMC = 1; // sample time = 125ns

    AD1CHS = 0;
    AD1CHSbits.CH0SA = 0; // channel selected is channel 0 (doesn't matter for scan)

    // select pins AN1, AN2, AN3 and AN9 as analog
    AD1PCFGbits.PCFG0 = 0;
    AD1PCFGbits.PCFG1 = 0;
    AD1PCFGbits.PCFG2 = 0;
    AD1PCFGbits.PCFG3 = 0;
    AD1PCFGbits.PCFG4 = 0;

    // This time we scan AN1, AN2, AN3, and AN9
    AD1CSSLbits.CSSL0 = 1;
    AD1CSSLbits.CSSL1 = 1;
    AD1CSSLbits.CSSL2 = 1;
    AD1CSSLbits.CSSL3 = 1;
    AD1CSSLbits.CSSL4 = 1;

    // now turn on A/D
    AD1CON1bits.ADON = 1;
    
    T3CON = 0;
    TMR3 = 0;
    PR3 = 34; // is this fast enough?
    T3CONbits.TON = 1;
    IEC0bits.AD1IE = 1; // enable interrupt on ADC completion
    IFS0bits.AD1IF = 0;
}
void setup_switch(void){
    
    TRISAbits.TRISA2=1;
    TRISAbits.TRISA3=1;
    TRISAbits.TRISA4=1;
    TRISBbits.TRISB4=1;
    
    CNPU2bits.CN30PUE = 1; // set pull up resistor of PORTAbits.RA2
    CNPU2bits.CN29PUE = 1; // set pull up resistor of PORTAbits.RA3
    CNPU1bits.CN1PUE = 1; // set pull up resistor of PORTAbits.RB4
    CNPU1bits.CN0PUE = 1; // set pull up resistor of PORTAbits.RA4
    
    T1CON = 0x0030; // turn off,
    PR1 = 7000;  // combined with 1:64 ==> 1,600,000, which is 100ms
    TMR1 = 0;
    _T1IF = 0;
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1;// Turn on OC  
   
}
void user_volatge(void){
    int i=0;
    if (ON_OFF == 0) {
        power ^= 1;
    }
    if (power == 1) {
        if(outputX==0){
            //output_x =(output_x+1)& 4;
            if (output_x<4){
            output_x++;
            }else{
            output_x=0;
            }
        }
        if(incV==0){
            switch (output_x) {
                case 0:
                if(v_set[0]<limit_HV){
                    v_set[0]=v_set[0]+0.1;
                    V_SET[0]=(v_set[0]/16.94)*3.33;
                }   
                break;
                case 1:
                    if(v_set[1]<limit_HV){
                        v_set[1]=v_set[1]+0.1;
                        V_SET[1]=(v_set[1]/16.94)*3.33;
                    }
                break;
                case 2:
                    if(v_set[2]<limit_HV){
                        v_set[2]=v_set[2]+0.1;
                        V_SET[2]=(v_set[2]/16.94)*3.33;
                    }
                break;
                case 3:
                    if(v_set[3]<limit_HV){
                        v_set[3]=v_set[3]+0.1;
                        V_SET[3]=(v_set[3]/16.94)*3.33;
                    }
                break;
                case 4:
                    if(v_set[4]<limit_HV){
                        v_set[4]=v_set[4]+0.1;
                        V_SET[4]=(v_set[4]/16.94)*3.33;
                    
                    }
                break;
                default:
                    for (i=0;i<5;i++){
                        V_SET[i]=0;
                    }             
            }        
        }
        if(decV==0){
            switch (output_x) {
                case 0:
                if(v_set[0]>limit_LV){
                    v_set[0]=v_set[0]-0.1;
                    V_SET[0]=(v_set[0]/16.94)*3.33;
                }
                break;
                case 1:
                if(v_set[1]>limit_LV){    
                    v_set[1]=v_set[1]-0.1;
                    V_SET[1]=(v_set[1]/16.94)*3.33;
                }
                break;
                case 2:
                if(v_set[2]>limit_LV){
                    v_set[2]=v_set[2]-0.1;
                    V_SET[2]=(v_set[2]/16.94)*3.33;
                }
                break;
                case 3:
                if(v_set[3]>limit_LV){
                    v_set[3]=v_set[3]-0.1;
                    V_SET[3]=(v_set[3]/16.94)*3.33;
                }
                break;
                case 4:
                if(v_set[4]>limit_LV){
                    v_set[4]=v_set[4]-0.1;
                    V_SET[4]=(v_set[4]/16.94)*3.33;
                }
                break;
                default:
                    for (i=0;i<5;i++){
                        V_SET[i]=0;
                    }     
            }        
        }
    }
}

void Display(void){
    if (power == 1) {
            switch (output_x) {
            case 0:
                    lcd_cmd(0b00000001); // Clear Display
                    delay1ms(2);
                    sprintf(adStr, "%2.1f  SA", (1.0) * v_set[0]); // ?x.xxxx V?
                    lcd_setCursor(0, 0);
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    sprintf(adStr, "%2.1f  RA", v_read[0]*(16.94/3.33)*(3.33/1024)); // ?x.xxxx V?
                    lcd_setCursor(1, 0);
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                break;
            case 1:
                    lcd_cmd(0b00000001); // Clear Display
                    delay1ms(2);
                    sprintf(adStr, "%2.1f  SB", v_set[1]); // ?x.xxxx V?
                    lcd_setCursor(0, 0);
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    sprintf(adStr, "%2.1f  RB", v_read[1]*(16.94/3.33)*(3.33/1024)); // ?x.xxxx V?
                    lcd_setCursor(1, 0);
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                break;
            case 2:
                    lcd_cmd(0b00000001); // Clear Display
                    delay1ms(2);
                    sprintf(adStr, "%2.1f  SC", v_set[2]); // ?x.xxxx V?
                    lcd_setCursor(0, 0);
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    sprintf(adStr, "%2.1f  RC", v_read[2]*(16.94/3.33)*(3.33/1024)); // ?x.xxxx V?
                    lcd_setCursor(1, 0);
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                break;
            case 3:
                lcd_cmd(0b00000001); // Clear Display
                    delay1ms(2);
                    sprintf(adStr, "%2.1f  SD", v_set[3]); // ?x.xxxx V?
                    lcd_setCursor(0, 0);
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                    sprintf(adStr, "%2.1f  RD", v_read[3]*(16.94/3.33)*(3.33/1024)); // ?x.xxxx V?
                    lcd_setCursor(1, 0);
                    lcd_printStr(adStr); //lcd_printStr(setTemp);
                break;
            case 4:
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                sprintf(adStr, "%2.1f  SE", v_set[4]); // ?x.xxxx V?
                lcd_setCursor(0, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                sprintf(adStr, "%2.1f  RE", v_read[4]*(16.94/3.33)*(3.33/1024)); // ?x.xxxx V?
                lcd_setCursor(1, 0);
                lcd_printStr(adStr); //lcd_printStr(setTemp);
                break;
            default:
                lcd_cmd(0b00000001); // Clear Display
                delay1ms(2);
                lcd_setCursor(0, 0);
                lcd_printStr("OFF");
            }
    }
    else{
        lcd_cmd(0b00000001); // Clear Display
        delay1ms(2);
        lcd_setCursor(0, 0);
        lcd_printStr("OFF");
    }
}
int main() {
    setuplcd();
    lcd_init();
    setup_OCRX();
    setup_adc_channels();
    setup_switch();
    power=1;///**///////////////////////////////DELETEEEEEEEEEEE
    while(1){
      asm("nop");  
    }
    return 0;
}
