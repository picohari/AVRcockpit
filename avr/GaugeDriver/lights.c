

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>

#include "config.h"
#include "timer.h"

#include "lights.h"


uint8_t lights_status = 0;


void lights_init(void)
{
	/* 12V Switches for LIGHTS (UDN2981A) */
	DDRA = 0xFF;
	PORTA = 0x00;
}


void lights_illumination(uint8_t status)
{
	if (status == 0) {
		// Switch illumination off
		PORTA &= ~(1 << LIGHTS_ILLUMINATION);
	} else {
		// Switch illumination on
		PORTA |= (1 << LIGHTS_ILLUMINATION);
	}

}


void lights_highbeam(uint8_t status)
{
	if (status == 0) {
		// Switch highbeam off
		PORTA &= ~(1 << LIGHTS_HIGHBEAM);
	} else {
		// Switch highbeam on
		PORTA |= (1 << LIGHTS_HIGHBEAM);
	}

}


static uint16_t lightsblinker_ticks = 0;
static uint8_t lightsblinker_toggle = 0;

void lights_blinker(unsigned short timing, unsigned char mode)
{
	if (timer_ms_passed(&lightsblinker_ticks, timing)) {		// Set blinker toggle frequency depending on input
			
		if (timing != 0) {

			if (lightsblinker_toggle == 1){
		
				PORTA &= ~(1 << LIGHTS_TURN_LEFT);
				PORTA &= ~(1 << LIGHTS_TURN_RIGHT);
	 			lightsblinker_toggle = 0;
 			 	 			
			} else {

				switch (mode) {						// Mode determines wich lights will blink (left, right both)
					case 0:
						PORTA |= (1 << LIGHTS_TURN_LEFT);
						PORTA |= (1 << LIGHTS_TURN_RIGHT);
						break;
					case 1:
						PORTA |= (1 << LIGHTS_TURN_LEFT);
						break;
					case 2:
						PORTA |= (1 << LIGHTS_TURN_RIGHT);
						break;
				}

				lightsblinker_toggle = 1;
			}

		} else {									// Turn all lights off if timing == 0
			PORTA &= ~(1 << LIGHTS_TURN_LEFT);
			PORTA &= ~(1 << LIGHTS_TURN_RIGHT);
			lightsblinker_toggle = 0;
		}	 			
	}

}







