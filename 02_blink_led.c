/** C O N F I G U R A T I O N   B I T S ******************************/                                             // CONFIG7H
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                       // CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30                        // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                    // CONFIG2H
#pragma config MCLRE = OFF, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC    ,HFOFST=OFF        // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF							      // CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF                   // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                         // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF                // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                            // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF           // CONFIG7L
#pragma config EBTRB = OFF                                                  // CONFIG7H

/** I N C L U D E S **************************************************/
#include <xc.h>

/** D E F I N I T I O N S ****************************************************/
#define KHz *1000UL
#define MHz *1000000UL
#define _XTAL_FREQ 8 MHz  //define crystal frequency

void main(void) 
{
    IDLEN = 0;// Device enters Sleep mode on SLEEP instruction
    IRCF2 = 1;//8 MHz
    IRCF1 = 1;//8 MHz
    IRCF0 = 0;//8 MHz
    OSTS = 0;// Device is running from the internal oscillator
    IOFS = 1;// HFINTOSC frequency is stable
    SCS1 = 1;//1x = Internal oscillator block
    SCS0 = 0;//1x = Internal oscillator block
//    OSCCON= 0b01100110;

    TRISD = 0b01111111; 	// PORTD bit 7 to output (0) ; bits 6:0 are inputs (1) 
    while(1)
    {
        LATDbits.LATD7 = ~LATDbits.LATD7; // toggle LATD
//        XTAL = 1 MHz => 1/1000 s/clock => 4/1000 s / instruction (because 4 clicks / instruction))
//        => 4/1000 s / instruction * (1000 instructions) = 1 s
//        see #define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
        __delay_ms(1000/2);// to get 1/2 sec by light on (or lignt off))
    }
    return;
}