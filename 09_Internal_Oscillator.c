/** C O N F I G U R A T I O N   B I T S ******************************/
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                      // CONFIG1H
#pragma config PWRT = OFF, BOREN = OFF, BORV = 30                           // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                   // CONFIG2H
#pragma config MCLRE = ON, LPT1OSC = OFF, PBADEN = OFF, CCP2MX = PORTC      // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF                          // CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF                   // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                         // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF               // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                           // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF           // CONFIG7L
#pragma config EBTRB = OFF                                                  // CONFIG7H

/** I N C L U D E S **************************************************/
#include <xc.h>

/** D E F I N I T I O N S ****************************************************/
#define KHz *1000UL
#define MHz *1000000UL
#define _XTAL_FREQ 1 MHz  //define crystal frequency

/** D E C L A R A T I O N S **************************************************/
typedef enum {
    C_250kHz = 0,
    C_500kHz = 1,
    C_1MHz = 2,
    C_2MHz = 3,
    C_4MHz = 4,
    C_8MHz = 5,
    C_16MHz = 6,
    C_32MHz = 7,
    C_64MHz = 8
} IntOSCFreq;
               
/** V A R I A B L E S *************************************************/
unsigned char LED_Count = 0;    // 8-bit variable
unsigned char SwitchDelay = 1;  // delay time to "debounce" switch
IntOSCFreq ClockSpeed = C_250kHz;

/** P R O T O T Y P E S ******************************************************/
void InterruptService(void);
void SetupINT0Switch(void);
void EnableInterrupts(void);
void SetIntOSC(IntOSCFreq *ClockSet);



void main (void)
{
  // Init I/O
    TRISD = 0b00000000;     	// PORTD bits 7:0 are all outputs (0)
	TRISEbits.TRISE0 = 1;		// TRISE0 input

	INTCON2bits.RBPU = 0;		// enable PORTB internal pullups
	WPUBbits.WPUB0 = 1;			// enable pull up on RB0
    // Set initial clock speed (250kHz)
    SetIntOSC(&ClockSpeed);

    // Init switch and turn on interrupts
    SetupINT0Switch();
    EnableInterrupts();

    while (1)
    { // delay and count on LEDs here.  Interrupt handles switch and freq changes

        LATD = LED_Count++;         // output count to PORTD LEDs
        __delay_ms(100);            // delay 64,000 cycles or about 1 sec at 250kHz 
    }
	
}

void SetupINT0Switch(void)
{   // Set up switch interrupt on INT0
    INTCON2bits.INTEDG0 = 0;    // interrupt on falling edge of INT0 (switch pressed)
    INTCONbits.INT0IF = 0;      // ensure flag is cleared
    INTCONbits.INT0IE = 1;      // enable INT0 interrupt
}

void EnableInterrupts(void)
{   // Set up global interrupts
    RCONbits.IPEN = 0;          // Disable priority levels on interrupts
    INTCONbits.PEIE = 1;        // Peripheral interrupts allowed (but none are used)
    INTCONbits.GIE = 1;         // Interrupting enabled.
}

void SetIntOSC(IntOSCFreq *ClockSet)
{   // This function sets the internal oscillator to the frequency of
    // the ClockSet argument variable, and then increments ClockSet
    // to the next supported frequency.
    switch (*ClockSet)
    {
//        REGISTER 2-1: OSCCON: OSCILLATOR CONTROL REGISTER
        case C_250kHz:
            OSCCON = 0x10;          // IRCFx = 001
            OSCTUNEbits.PLLEN = 0;  // x4 PLL disabled
            *ClockSet = C_500kHz;
            SwitchDelay = 1;
            break;

        case C_500kHz:
            OSCCON = 0x20;          // IRCFx = 010
            OSCTUNEbits.PLLEN = 0;  // x4 PLL disabled
            *ClockSet = C_1MHz;
            SwitchDelay = 2;
            break;

        case C_1MHz:
            OSCCON = 0x30;          // IRCFx = 011
            OSCTUNEbits.PLLEN = 0;  // x4 PLL disabled
            *ClockSet = C_2MHz;
            SwitchDelay = 4;
            break;

        case C_2MHz:
            OSCCON = 0x40;          // IRCFx = 100
            OSCTUNEbits.PLLEN = 0;  // x4 PLL disabled
            *ClockSet = C_4MHz;
            SwitchDelay = 8;
            break;

        case C_4MHz:
            OSCCON = 0x50;          // IRCFx = 101
            OSCTUNEbits.PLLEN = 0;  // x4 PLL disabled
            *ClockSet = C_8MHz;
            SwitchDelay = 16;
            break;

        case C_8MHz:
            OSCCON = 0x60;          // IRCFx = 110
            OSCTUNEbits.PLLEN = 0;  // x4 PLL disabled
            *ClockSet = C_16MHz;
            SwitchDelay = 32;
            break;

        case C_16MHz:
            OSCCON = 0x70;          // IRCFx = 111
            OSCTUNEbits.PLLEN = 0;  // x4 PLL disabled
            *ClockSet = C_32MHz;
            SwitchDelay = 64;
            break;

        case C_32MHz:
            OSCCON = 0x60;          // IRCFx = 110 (8 MHz)
            OSCTUNEbits.PLLEN = 1;  // x4 PLL enabled = 32MHz
            *ClockSet = C_64MHz;
            SwitchDelay = 128;
            break;

        case C_64MHz:
            OSCCON = 0x70;          // IRCFx = 111 (16 MHz)
            OSCTUNEbits.PLLEN = 1;  // x4 PLL enabled = 64MHz
            *ClockSet = C_250kHz;
            SwitchDelay = 255;
            break;

        default:
            // should never get here, but just in case
            OSCCON = 0x10;          // IRCFx = 001
            OSCTUNEbits.PLLEN = 0;  // x4 PLL disabled
            *ClockSet = C_500kHz;
            break;
    }
}

/** I N T E R R U P T S ***********************************************/
//non-reentrant function
void __interrupt (high_priority) myHiIsr(void)
{
    int i;

    // Check to see what caused the interrupt
    // (Necessary when more than 1 interrupt at a priority level)

    // Check for INT0 interrupt
    if (INTCONbits.INT0IF)
    {
        // Delay about 50ms regardless of frequency to debounce switch.
        // NOTE: a delay function from delay.h isn't used as they are 
        // inline assembly and inline assembly in an interrupt routine
        // causes a very long context save/restore since the compiler
        // doesn't know what resources are being used and so saves everything.
        i = 125 * SwitchDelay;
        while(i)
        {
            i--;
        }
        

        // clear (reset) flag
        INTCONbits.INT0IF = 0;

        if (PORTBbits.RB0 == 0)
        { // if it's still pressed after 100ms delay.
            // Change oscilator frequency
            SetIntOSC(&ClockSpeed);
        }
    }
    // Check for another interrupt, examples:
    // if (PIR1bits.TMR1IF)     // Timer 1
    // if (PIR1bits.ADIF)       // ADC
}  // return from high-priority interrupt


//09_Internal_Oscillator.c:86:: advisory: (1510) non-reentrant function "_SetIntOSC" appears in multiple call graphs and has been duplicated by the compiler
//Non-reentrant functions are functions that cannot safely be called, interrupted, and then 
//recalled before the first call has finished without resulting in memory corruption.