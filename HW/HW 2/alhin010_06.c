int main(void) {
    
    int RB2 = 0;
    int i;

    T1CON = 0;
    PR1 = 15999;
    IFS0bits.T1IF = 0;
    TMR1 = 0;
    T1CONbits.TON = 1;
    
    while (1){
        for (i=0; i < 3000; i++){
                while (IFS0bits.T1IF==0);
                    IFS0bits.T1IF = 0;
        }
        RB2 ^=1;
    }
}