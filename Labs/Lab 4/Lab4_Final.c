#include <p24Fxxxx.h>
#include <xc.h>

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


//Number of times TMR2 rolled over for button 1 and 2 
volatile unsigned int overflow_1 = 0;   
volatile unsigned int overflow_2 = 0;

//Saving the Last 4 presses of button 1 and 2
volatile unsigned long int BUFF1[4] = {0, 0, 0, 0};
volatile unsigned long int BUFF2[4] = {0, 0, 0, 0};

//Track the number of presses in one period for button 1 and 2 
volatile int i = 0; 
volatile int j = 0;

//Take the average of time for presses of button 1 and 2
volatile unsigned long int Ave1 = 0;
volatile unsigned long int Ave2 = 0;

//Adjust the position of the servo starting at zero degree
unsigned int Servo = 3000;

//Make a move in the servo if any of the buttons have been pressed 4 times
unsigned int turn = 0;




//Adjust the position of the servo
void setServo(int Val)
{
    OC1RS = Val;   
}


//Count the over flow of TMR2 for each button and make a move
//if any of the buttons have been pressed 4 times
void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;  // could have used _T2IF
    overflow_1++;
    overflow_2++;
    
    // adjust the movement of the servo according to the number of presses of button 1 and 2
    static long int speed;
    
    
    // make a move if any of the buttons have been pressed 4 times
    if (turn){
        // shift to the right if button 1 has been pressed faster than button 1
        if (Ave1 < Ave2) {
            speed = 200 * (Ave2-Ave1)/(Ave2+Ave1);
            Servo += speed;
            setServo(Servo);
            
            //if servo reaches +90 degree, player 1 wins, the right LED toggled, and the servo sets to zero degree 
            if (Servo > 4800) {
                _LATB10 ^= 1;
                Servo = 3000;
                setServo(Servo);
            }
            
        // shift to the left if button 2 has been pressed faster than button 1
        }else if (Ave1 > Ave2) {
            speed = 200 * (Ave1-Ave2)/(Ave2+Ave1);
            Servo -= speed;
            setServo(Servo);

            //if servo reaches -90 degree, player 2 wins, the left LED toggled, and the servo sets to zero degree 
            if (Servo < 1200) {
                _LATB11 ^= 1; // When it hits 1500, RB11 lit up to announce
                Servo = 3000;
                setServo(Servo);
            }
        }
        
        //reset everything to zero to prepare for another movement
        turn = 0;
        Ave1 = 0;
        Ave2 = 0;
    }
}

void __attribute__((__interrupt__,__auto_psv__))  _IC1Interrupt() 
{
    _IC1IF = 0;
    
    //save the time period between two presses
    static unsigned int prevEdge=0;
    int curEdge;
    curEdge = TMR2 + 6250L * overflow_1;
    
    //recored presses after 2 ms from the last press
    if ((curEdge - prevEdge) > 125){
        BUFF1[i] = curEdge - prevEdge;
        prevEdge = curEdge;
        overflow_1 = 0;

        //Average out the last 4 presses period
        Ave1 = (BUFF1[0]+ BUFF1[1]+ BUFF1[2]+ BUFF1[3])/4;  
        
        // move to same another press
        i++;
        i %= 4;

        //make the servo move if it completes 4 turns 
        if (!i)
            turn = 1;
    }
}

void __attribute__((__interrupt__,__auto_psv__)) _IC2Interrupt() 
{
    _IC2IF = 0;
    
    //save the time period between two presses
    static unsigned int prevEdge=0;
    int curEdge;
    curEdge = TMR2 + 6250L * overflow_2;
    
    //recored presses after 2 ms from the last press
    if ((curEdge - prevEdge) > 125){
        BUFF2[j] = curEdge - prevEdge;
        prevEdge = curEdge;
        overflow_2 = 0;

        //Average out the last 4 presses period
        Ave2 = (BUFF2[0]+ BUFF2[1]+ BUFF2[2]+ BUFF2[3])/4;
        
        // move to same another press
        j++;
        j %= 4;

        //make the servo move if it completes 4 turns
        if (!j)
            turn = 1;
    }
}


void initServo(void)
{

    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;  // For digital I/O.
    TRISB = 0;

    T3CON = 0x0010; // turn off, 1:8 pre, Tcy clock source

    PR3 = 40000;
    TMR3 = 0;
    _T3IF = 0;
   
    OC1CON = 0;
    OC1R = 20; 
    OC1RS = 0; // We will only change this once PWM is turned on
    OC1CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC1CONbits.OCTSEL = 1; // Use Timer 3 for compare source

     __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    //RPINR7bits.IC1R = 9; // Use Pin RP9 = "9", for Input Capture 1 (Table 10-2)
    RPOR3bits.RP6R = 18; // Use Pin RP6 for Output Compare 1 = "18" (Table 10-3)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
    
     T3CONbits.TON = 1;
 }

void initTwoPushButtons(void)
{
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;  // For digital I/O.
    TRISB = 0x0300;
    
    _TRISB8 = 1;  // assign RB8 to be an input
    _TRISB9 = 1;  // assign RB9 to be an input
    _CN22PUE = 1; // enable pull up resistor on pin RB8
    _CN21PUE = 1; // enable pull up resistor on pin RB9

    _TRISA0 = 0;  // assign RA0 to be an output
    _TRISA1 = 0;  // assign RA1 to be an output
    _LATA0 = 0;   // set RA0 to zero
    _LATA1 = 0;   // set RA1 to zero
    
    T2CON = 0x8030; // turn off, 1:256 pre, Tcy clock source
    PR2 = 6250;     // refresh every 100 ms 
    TMR2 = 0;
    _T2IF = 0;
    _T2IE = 1;
    
    
    IC1CON = 0; // Turn off and reset internal state of IC1
    IC1CONbits.ICTMR = 1; // Use Timer 2 for capture source
    IC1CONbits.ICM = 0b010; // Turn on and capture every falling edge
    IFS0bits.IC1IF = 0; //sets flag to 0
    IEC0bits.IC1IE = 1; //enables the interrupt
    
    IC2CON = 0; // Turn off and reset internal state of IC2
    IC2CONbits.ICTMR = 1; // Use Timer 2 for capture source
    IC2CONbits.ICM = 0b010; // Turn on and capture every falling edge
    IFS0bits.IC2IF = 0; //sets flag to 0
    IEC0bits.IC2IE = 1; //enables the interrupt
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR7bits.IC1R = 8; // Use Pin RP8 = "8", for Input Capture 1 (Table 10-2)
    RPINR7bits.IC2R = 9; // Use Pin RP9 = "9", for Input Capture 2 (Table 10-2)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
}

int main(void) {
    
    //set the setting for the servo and the two buttons
    initServo();
    initTwoPushButtons();
    
    //set the servo initially to zero degree position 
    setServo(3000);
    
    while(1);
    
    return 0;
}