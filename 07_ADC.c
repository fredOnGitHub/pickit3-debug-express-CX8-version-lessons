/** C O N F I G U R A T I O N   B I T S ******************************/
#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                       // CONFIG1H
#pragma config PWRT = OFF, BOREN = OFF, BORV = 30                        // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                     // CONFIG2H
#pragma config MCLRE = ON, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC       // CONFIG3H
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
#define Switch_Pin      PORTBbits.RB0
#define DetectsInARow   5

/** D E C L A R A T I O N S **************************************************/
typedef enum { LEFT2RIGHT, RIGHT2LEFT} LEDDirections;
typedef enum {FALSE, TRUE} BOOL;
unsigned char LED_Display;  // 8-bit variable

/** P R O T O T Y P E S ******************************************************/
void Timer0_Init(void);
void ADC_Init(void);
unsigned char ADC_Convert(void);

void main (void)
{
    LEDDirections Direction = LEFT2RIGHT;
    BOOL SwitchPressed = FALSE;

    LED_Display = 1;            // initialize

    // Init I/O
    TRISD = 0b00000000;     	// PORTD bits 7:0 are all outputs (0). p 122
	TRISAbits.TRISA0 = 1;		// TRISA0 input. p 188

	INTCON2bits.RBPU = 0;		// enable PORTB internal pullups; p 103 & 116
	WPUBbits.WPUB0 = 1;			// enable pull up on RB0. 
//    @ at F7C see pic18f45k20.h and p 74 
//    (SPECIAL FUNCTION REGISTER MAP FOR PIC18F2XK20/4XK20 DEVICES
//    Address Name Address Name Address Name Address ...)

    // ADCON1 is now set up in the InitADC() function.
    TRISBbits.TRISB0 = 1;       // PORTB bit 0 (connected to switch) is input (1)

    // Init Timer0
    Timer0_Init();

    // Init ADC
    ADC_Init();

    while (1)
    {

        if (Direction == LEFT2RIGHT)
        {
            LED_Display <<= 1;          // rotate display by 1 from 0 to 7
            if (LED_Display == 0)
                LED_Display = 1;        // rotated bit out, so set bit 0
        }
        if (Direction == RIGHT2LEFT)
        {
            LED_Display >>= 1;          // rotate display by 1 from 7 to 0
            if (LED_Display == 0)
                LED_Display = 0x80;     // rotated bit out, so set bit 7
        }

        LATD = LED_Display;         // output LED_Display value to PORTD LEDs

        do
        { // poll the switch while waiting for the timer to roll over.
            if (Switch_Pin == 1)
            { // look for switch released.
                SwitchPressed = FALSE;
            }
            else if (SwitchPressed == FALSE) // && (Switch_Pin == 0) due to if-else
            { // switch was just pressed
                SwitchPressed = TRUE;
                // change  direction
                if (Direction == LEFT2RIGHT)
                    Direction = RIGHT2LEFT;
                else
                    Direction = LEFT2RIGHT;
            }

        } while (INTCONbits.TMR0IF == 0);

        // Timer expired
        INTCONbits.TMR0IF = 0;          // Reset Timer flag

        // Take an ADC conversion and use it to set Timer0
        TMR0H = ADC_Convert();      // MSB from ADC
        TMR0L = 0;                  // LSB = 0

    }
	return;
}

void Timer0_Init(void)
{
//    p 145
    INTCONbits.TMR0IF = 0;          // clear roll-over interrupt flag
    T0CON = 0b00000001;             // prescale 1:4 - about 1 second maximum delay.
    TMR0H = 0;                      // clear timer - always write upper byte first
    TMR0L = 0;
    T0CONbits.TMR0ON = 1;           // start timer
}

void ADC_Init(void)
{ // initialize the Analog-To-Digital converter.
    // First, we need to make sure the AN0 pin is enabled as an analog input
    // as the demo board potentiometer is connected to RA0/AN0
    // Don't forget that RB0/AN12 must be digital!
	ANSEL = 0;	//turn off all other analog inputs
	ANSELH = 0;
    
//    ANSEL port B => bit 7-0 = ANS7-0 : p 128 
//    ANSELH port B => bit 3-0 = ANS12-8 : p 118 
    
 	ANSELbits.ANS0 = 1;	// turn on RA0 analog

    // Sets bits VCFG1 and VCFG0 in ADCON1 so the ADC voltage reference is VSS to VDD

//    19.1.3 ADC VOLTAGE REFERENCE
//    The VCFG bits of the ADCON1 register provide
//    independent control of the positive and negative
//    voltage references. The positive voltage reference can
//    be either VDD or an external voltage source. Likewise,
//    the negative voltage reference can be either VSS or an
//    external voltage source
//    Set the ADC voltage references in ADCON1. 
    ADCON1 = 0;
    
    
    
//    VOIR
//    TABLE 26-36: A/D CONVERSION REQUIREMENTS 
//    130 TAD A/D Clock Period : p 386

//    REGISTER 19-3: ADCON2: A/D CONTROL REGISTER 2
//    ADCS<2:0>: A/D Conversion Clock Select bits
//    000 = FOSC/2
    
//    19.1.2 CHANNEL SELECTION
//    The CHS bits of the ADCON0 register determine which
//    channel is connected to the sample and hold circuit.
//    When changing channels, a delay is required before
//    starting the next conversion. Refer to Section 19.2
//    ?ADC Operation? for more information
//    19.2 ADC Operation
//    19.2.1 STARTING A CONVERSION
//    To enable the ADC module, the ADON bit of the
//    ADCON0 register must be set to a ?1?. Setting the GO/
//    DONE bit of the ADCON0 register to a ?1? will, depending on the ACQT bits of the ADCON2 register, either
//    immediately start the Analog-to-Digital conversion or
//    start an acquisition delay followed by the Analog-toDigital conversion.    
    
    
    
    
    // The ADC clock must as short as possible but still greater than the
    // minimum TAD time, datasheet parameter 130.  At the time this lesson was
    // written TAD minimum for the PIC18F45K20 is 1.4us.
    // At 1MHz clock, selecting ADCS = FOSC/2 = 500kHz.  One clock period
    // 1 / 500kHz = 2us, which greater than minimum required 1.4us.
    // So ADCON2 bits ADCS2-0 = 000
    //
    // The ACQT aquisition time should take into accound the internal aquisition
    // time TACQ of the ADC, datasheet paramter 130, and the settling time
    // of the application circuit connected to the ADC pin.  Since the actual
    // settling time of the RC circuit with the demo board potentiometer is very
    // long but accuracy is not very important to this demo, we'll set ACQT2-0 to
    // 20TAD = 111
    //
    // ADFM = 0 so we can easily read the 8 Most Significant bits from the ADRESH
    // Special Function Register
    ADCON2 = 0b00111000;

    // Select channel 0 (AN0) to read the potentiometer voltage and turn on ADC
    ADCON0 = 0b00000001;
}

unsigned char ADC_Convert(void)
{ // start an ADC conversion and return the 8 most-significant bits of the result
    
//    ADRESH => FIGURE 19-2: 10-BIT A/D CONVERSION RESULT FORMAT 
//    REGISTER 19-1: ADCON0: A/D CONTROL REGISTER 0
//    ADCON0 == bit 1 de ADCON0
            
    ADCON0bits.GO_DONE = 1;             // start conversion
    while (ADCON0bits.GO_DONE == 1);    // wait for it to complete
    return ADRESH;                      // return high byte of result
}