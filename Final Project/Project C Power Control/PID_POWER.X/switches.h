
#ifndef SWITCHES_H
#define	SWITCHES_H
  


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define ON_OFF  PORTBbits.RB4
#define outputX PORTAbits.RA4
#define decV    PORTAbits.RA3
#define incV    PORTAbits.RA2
#define limit_HV 10
#define limit_LV 0

#define KP 0.5
#define KI 0.5
#define KD 0.5
    
void user_volatge(void);
void setuplcd();
void lcd_init();
void setup_OCRX();
void setup_adc_channels();
void setup_switch();

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

