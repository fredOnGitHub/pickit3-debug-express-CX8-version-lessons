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
#define Switch_Pin PORTBbits.RB0// RB0 only is deprecated 
#define DetectsInARow 5
#define KHz *1000UL
#define MHz *1000000UL
#define _XTAL_FREQ 1 MHz  //define crystal frequency (by default)

/** V A R I A B L E S *************************************************/
unsigned char LED_Display;  // 8-bit variable

void main(void) {
    
    unsigned char Switch_Count = 0;

    LED_Display = 1;            // initialize

    TRISD = 0b00000000;     	// PORTD bits 7:0 are all outputs (0)
	RBPU = 0;		// enable PORTB internal pullups
	WPUB0 = 1;			// enable pull up on RB0
    ANSELH = 0x00;              // AN8-12 are digital inputs (AN12 on RB0)
    TRISB0 = 1;       // PORTB bit 0 (connected to switch) is input (1)

    while (1)
    {
        LATD = LED_Display;     // output LED_Display value to PORTD LEDs

        LED_Display <<= 1;      // rotate display by 1

        if (LED_Display == 0)
            LED_Display = 1;    // rotated bit out, so set bit 0

        
        while (Switch_Pin != 1);// wait for switch to be released

        Switch_Count = 5;
        do
        { // monitor switch input for 5 lows in a row to debounce
            if (Switch_Pin == 0)
            { // pressed state detected
                Switch_Count++;
            }
            else
            {
                Switch_Count = 0;
            }   
            __delay_ms(1);
        } while (Switch_Count < DetectsInARow); 
    }
	
    return;
}
