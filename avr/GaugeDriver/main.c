
/*
 *  AVRcockpit Development System for Racing Simulators
 *
 *  Harald W. Leschner, DK6YF - 2015  H9-Laboratory Ltd. 
 *
 *     hari@h9l.net    or visit    labor.h9l.net
 *
 *
 *  Licensed under the GPL license, see license.txt for details.
 *
 *     All text above must be included in any redistribution.
 *
 */

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "config.h"
//#include "sim-commands.h"

#include "../../common/datacodes.h"

#include "timer.h"
#include "font5x7.h"
#include "twi-slave.h"

#include "gear-indicator.h"
#include "lights.h"
#include "gauges.h"


/* For casting transmitted values */
//bool t_bool = FALSE;		// temporary values for storing received VALUE of WHAT
char t_char = 0;
short t_short = 0;
long t_long = 0;
float t_float = 0;
double t_double = 0;
uint8_t t_fourbyte[4] = {0,0,0,0};

/* Local static variables for background running applications */
static unsigned int  gear_data = 0;				// Gear indicator command
static unsigned short blinker_timing = 0;		// Blinker timing command
static unsigned char blinker_mode = 1;			// Blinker mode command


/* I²C Receive buffer and slave address */
unsigned char messageBuf[TWI_BUFFER_SIZE];
unsigned char TWI_slaveAddress = TWI_ADDRESS;


// When there has been an error, this function is run and takes care of it
unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )
{
	return TWIerrorMsg; 
}


/* Initialization code */ 
static void init(void)
{

	/* All Inputs -> all zeros */
	PORTA=0; DDRA=0;
	PORTB=0; DDRB=0;
	PORTC=0; DDRC=0;
	PORTD=0; DDRD=0;
	PORTE=0; DDRE=0;
	PORTF=0; DDRF=0;
	PORTG=0; DDRG=0;

	wdt_disable();		// Watchdog off!


	/* LIGHTS Indicators */
	lights_init();

	/* GEAR Indicator */
	gear_indicator_init();

	/* ANALOG SPEED AND RPM Indicator */
	gauges_init();



	/* DEVICE Internal Timer */
	timer_2_init();

	/* I²C Slave Interface */
	#ifdef TWI_AVAILABLE
	// Initialise TWI module for slave operation. Include address and/or enable General Call.
	TWI_Slave_Initialise( (unsigned char)((TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) )); 
	#endif

}




/* Main routine */
int main (void)
{

	/* Initiate all */
	init();

	sei();

	#ifdef TWI_AVAILABLE
	// Start the TWI transceiver to enable reseption of the first command from the TWI Master.
	TWI_Start_Transceiver();
	#endif

	/* Show funny smiley on gear-indicator as welcome */
	glyph = ';';	// Blank
	glyph = '<';	// :)
	delay(2000);
	glyph = ';';	
	

	/* Main Loop */
	for(;;)
	{
	
		#ifdef TWI_AVAILABLE

		// Check if the TWI Transceiver has completed an operation.
		if ( ! TWI_Transceiver_Busy() )                              
		{
			// Check if the last operation was successful
			if ( TWI_statusReg.lastTransOK )
			{
				// Check if the last operation was a reception
				if ( TWI_statusReg.RxDataInBuf )
				{
					TWI_Get_Data_From_Transceiver(messageBuf, 4);		// Read number of bytes !!
         
					// Check if the last operation was a reception as General Call        
					if ( TWI_statusReg.genAddressCall )
					{
					// Put data received out to PORTB as an example.        
						//PORTB = messageBuf[0];
					}               
					else // Ends up here if the last operation was a reception as Slave Address Match   
					{

						/* Make analysis of WHAT was received and perform post-functionality */

						/* Set Gear */
						if (messageBuf[0] == GEAR)
						{
							gear_data = messageBuf[1];                            
						}

						/* Set toggle HEADLIGHTS */
						if (messageBuf[0] == HEADLIGHTS)
						{
							lights_illumination(messageBuf[1]);                            
						}

						/* Set toggle HIGHBEAM */
						if (messageBuf[0] == HIGHBEAM)
						{
							lights_highbeam(messageBuf[1]);                            
						}

						/* Set BLINKER light frequency and mode */
						if (messageBuf[0] == BLINKER)
						{
							blinker_timing = messageBuf[2] + (messageBuf[1] << 8);
							blinker_mode = messageBuf[3];
						}

						/* Set SPEED */
						if (messageBuf[0] == METERSPERSEC)
						{
							OCR1AH = messageBuf[1];		// Set values directly to timer OCR
							OCR1AL = messageBuf[2];

							TCNT1=0xffff;				// Update TCNT, as frequencies are low, it may take time to finish the cycle
														// thus we restart the timer TCNT every time to get proper frequency update
							if (OCR1A == 0xFFFF)
								gauge_speedo_off();		// Disconnect timer from clock to make a full stop of output.
							else
								gauge_speedo_on();
						}

						/* Set RPM */ 
						if (messageBuf[0] == ENGRPM)
						{
							OCR3AH = messageBuf[1];
							OCR3AL = messageBuf[2];

							TCNT3=0xffff;

							if (OCR3A == 0xFFFF)
								gauge_tacho_off();		// Disconnect timer from clock to make a full stop of output.
							else
								gauge_tacho_on();
						}

					}
				}                
				else // Ends up here if the last operation was a transmission  
				{
				    //NOP(); // Put own code here.
				}

				// Check if the TWI Transceiver has already been started.
				// If not then restart it to prepare it for new receptions.             
				if ( ! TWI_Transceiver_Busy() )
				{
				  TWI_Start_Transceiver();
				}
			}
			else // Ends up here if the last operation completed unsuccessfully
			{
				TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info() );
			}
		}

		#endif /* TWI_AVAILABLE */


		/* Display the current gear on indicator */ 
		gear_show(gear_data);

		/* Toggle warning lights by frequency */ 
		lights_blinker(blinker_timing, blinker_mode);




	

	
	} /* End for(;;) */ 

	/* If we ever should get here ;-) */
	return 1; 

}


