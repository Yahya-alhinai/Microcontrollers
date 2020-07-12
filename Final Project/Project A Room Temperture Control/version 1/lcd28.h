#ifndef LCD28_H
#define	LCD28_H


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

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

void wait(int number);
void lcd_cmd(char Package);
void lcd_init(void);
void lcd_setCursor(char x, char y);
void lcd_printChar(char Package);
void lcd_printStr(const char *s);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

