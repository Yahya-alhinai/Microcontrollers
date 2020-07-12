#ifndef FINAL_HEADER_H
#define	FINAL_HEADER_H

#ifdef	__cplusplus
extern "C" {
#endif


#pragma config ICS = PGx1
#pragma config FWDTEN = OFF
#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF
#pragma config POSCMOD = NONE
#pragma config I2C1SEL = PRI
#pragma config IOL1WAY = OFF
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSECME
#pragma config FNOSC = FRCPLL
    
    
#define ON_OFF PORTAbits.RA1
#define C_F PORTAbits.RA2
#define INC_T PORTAbits.RA3
#define DEC_T PORTBbits.RB4
#define STATE PORTBbits.RB14

#define FAN PORTBbits.RB5
#define HEATER PORTBbits.RB6
#define COOLER PORTBbits.RB7    
    
#define LIMIT_H_C 50
#define LIMIT_L_C 15
#define LIMIT_H_F 122
#define LIMIT_L_F 59
//#define SHUT_DOWN (1 * 10) //(x*10); where x is # seconds to shut down

void wait(int number);
void lcd_cmd(char Package);
void lcd_init(void);
void lcd_setCursor(char x, char y);
void lcd_printChar(char Package);
void lcd_printStr(const char *s);


void getTemp(void);
void convert_from_C_to_F_ActualTemp(unsigned char T);
void convert_from_C_to_F(unsigned char T);
void convert_from_F_to_C(unsigned char T);
void setup(void);


volatile char power;
volatile char convert_unit;
volatile char convert_STATE;
volatile float setTemp;
volatile float ActualTemp;
//volatile int count_Shut_Down = 0;
volatile int ADC_Buffer[1024];
volatile int i;
volatile char adStr[20];


#ifdef	__cplusplus
}
#endif

#endif	/* FINAL_HEADER_H */
