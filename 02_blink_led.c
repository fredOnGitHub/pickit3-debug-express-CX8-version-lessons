/** C O N F I G U R A T I O N   B I T S ******************************/                                             // CONFIG7H
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                       // CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30                        // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                    // CONFIG2H
#pragma config MCLRE = OFF, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC    //,HFOFST=OFF        // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF							      // CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF                   // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                         // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF                // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                            // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF           // CONFIG7L
#pragma config EBTRB = OFF                                                  // CONFIG7H

/** I N C L U D E S **************************************************/
#include <xc.h>


/*
 * p 317 CX8 compiler : void _delay(unsigned long cycles);
 * #define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
 * 1/FOSC = time/cycle
 * 1 instruction (ins) = 4 cycles
 * 4/FOSC = time/ins
 * 4/FOSC * FOSC/4 = unity
 * 1/1000=ins^(-1)
 * so void _delay(unsigned long cycles)
 */





/** D E C L A R A T I O N S **************************************************/

/** P R O T O T Y P E S ******************************************************/

/** V A R I A B L E S *************************************************/


/** D E F I N I T I O N S ****************************************************/
#define KHz *1000UL
#define MHz *1000000UL
#define _XTAL_FREQ 2 MHz  //define crystal frequency

void main(void) 
{
    TRISD = 0b01111111; 	// PORTD bit 7 to output (0) ; bits 6:0 are inputs (1)

//    REGISTER 2-1: OSCCON: OSCILLATOR CONTROL REGISTER
//    IDLEN IRCF2 IRCF1 IRCF0 OSTS(1) IOFS SCS1 SCS0
//    SCS0 = 0;//00 = Primary clock (determined by CONFIG1H[FOSC<3:0>]) for 32 & 64 MHz !!
//    SCS1 = 0;//00 = Primary clock (determined by CONFIG1H[FOSC<3:0>]) for 32 & 64 MHz !!

//    IOFS = 0;//  HFINTOSC frequency is not stable ok for 64 MHz 
//    IOFS = 1;// HFINTOSC frequency is stable ok for 64 MHz 
    
//    OSTS = 0;// Device is running from the internal oscillator
//    IRCFx = ??? see later
//    PLLEN=0;// x4 PLL disabled
//    IDLEN=0;// Device enters Sleep mode on SLEEP instruction

    OSCTUNEbits.PLLEN = 0;
    
#if _XTAL_FREQ == 31 KHz
    OSCCON = 0x00;
#elif _XTAL_FREQ == 250 KHz
    OSCCON = 0x10;     // IRCFx = 001
#elif _XTAL_FREQ == 500 KHz
    OSCCON = 0x20;     // IRCFx = 010
#elif _XTAL_FREQ == 1 MHz
    OSCCON = 0x30;     // IRCFx = 011
#elif _XTAL_FREQ == 2 MHz
    OSCCON = 0x40;     // IRCFx = 100
#elif _XTAL_FREQ == 4 MHz
    OSCCON = 0x50;     // IRCFx = 101
#elif _XTAL_FREQ == 8 MHz
    OSCCON = 0x60;     // IRCFx = 110
#elif _XTAL_FREQ == 16 MHz
    OSCCON |= 0x70;     // IRCFx = 111
#elif _XTAL_FREQ == 32 MHz
    OSCCON = 0x60;  // IRCFx = 110 (8 MHz) //before OSCTUNEbits.PLLEN = 1; !!!
    OSCTUNEbits.PLLEN = 1;  // x4 PLL enabled = 32MHz
#elif _XTAL_FREQ == 64 MHz
    OSCCON = 0x70;  // IRCFx = 111 (16 MHz) //before OSCTUNEbits.PLLEN = 1; !!!
    OSCTUNEbits.PLLEN = 1; // x4 PLL enabled = 64MHz
#else
    OSCCON = 0x30;     // IRCFx = 011
#endif

    while(1)
    {
        LATDbits.LATD7 = ~LATDbits.LATD7; // toggle LATD
//        XTAL = 1 MHz => 1/1000 s/clock => 4/1000 s / instruction (because 4 clicks / instruction))
//        => 4/1000 s / instruction * (1000 instructions) = unity
//        see #define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
        
        __delay_ms(500);//
    }
    return;
}
