/** C O N F I G U R A T I O N   B I T S ******************************/
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                       // CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30                        // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                     // CONFIG2H
#pragma config MCLRE = OFF, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC       // CONFIG3H
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
typedef unsigned char uchar; 

/** V A R I A B L E S *************************************************/
uchar LED_Number;  // 8-bit variable
const uchar LED_LookupTable[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

void main(void) {
//    uchar IDLEN = 0;// Device enters Sleep mode on SLEEP instruction
//    uchar IRCF = 0b011;// 1 MHz
//    uchar OSTS = 0;// Device is running from the internal oscillator
//    uchar IOFS = 1;// HFINTOSC frequency is stable
//    uchar SCS = 0b10;// 1x = Internal oscillator block
//    OSCCON= (uchar) (IDLEN<<7 | IRCF<<4 | OSTS<<3 | IOFS<<2 | SCS);
    
    IDLEN = 0;// Device enters Sleep mode on SLEEP instruction
    IRCF2 = 0;// 1 MHz
    IRCF1 = 1;// 1 MHz
    IRCF0 = 1;// 1 MHz
    OSTS = 0;// Device is running from the internal oscillator
    IOFS = 1;// HFINTOSC frequency is stable
    SCS1 = 1;// 1x = Internal oscillator block
    SCS0 = 0;// 1x = Internal oscillator block
//    OSCCON = 0b00110110;
            
    LED_Number = 0;            // initialize

    TRISD = 0b00000000;     	// PORTD bits 7:0 are all outputs (0)

    while (1)
    {
		// use lookup table to output one LED on based on LED_Number value
        LATD = LED_LookupTable[LED_Number];    

        LED_Number++;      // rotate display by 1

        if (LED_Number == 8)
            LED_Number = 0;    // go back to LED 0.

//        XTAL = 1 MHz => 1/1000 s/clock => 4/1000 s / instruction (because 4 clicks / instruction))
//        => 4/1000 s / instruction * (1000 instructions) = 1 s
//        see #define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
        __delay_ms(1000);
    }	
    return;
}
