/* 
 * File:   Lab06_header.h
 * Author: yahya
 *
 * Created on April 3, 2018, 9:44 PM
 */

#ifndef LAB06_HEADER_H
#define	LAB06_HEADER_H

#ifdef	__cplusplus
extern "C" {
#endif

void wait(int number);
void lcd_cmd(char Package);
void lcd_setCursor(int x, int y);
void lcd_init();
void lcd_printChar(char Package);
void lcd_printStr(const char *s);
void setup(void);



#ifdef	__cplusplus
}
#endif

#endif	/* LAB06_HEADER_H */

