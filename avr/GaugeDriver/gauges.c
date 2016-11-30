

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>

#include "config.h"
#include "timer.h"

#include "gauges.h"




void gauges_init(void)
{
	/* ANALOG GAUGES for Speed and RPM */ 
	// -- TIMER 1 and 3 set pins to OUTPUT
	DDRB |= (1 << PB5);
	DDRE |= (1 << PE3);

	// -- TIMER 1 and 3 set initial CTC @ 10bit
	OCR1A = 0xFFFF;
	OCR3A = 0xFFFF;

	/*
	// -- TIMER 1 Setting to PWM --
	// set none-inverting mode
	TCCR1A |= (1 << COM1A1);
	// set 8bit phase corrected PWM Mode
	TCCR1A |= (1 << WGM10);
	// set prescaler to 8 and starts PWM
	TCCR1B |= (1 << CS11);
	*/


	/* Speedometer (Tacho [for km/h]) */
	// -- TIMER 1 Setting to CTC --
	// set CTC mode (connect pins to OCR1x)
	TCCR1A |= ( (1 << COM1A0) | (1 << COM1B0) );
	// set CTC Immediate Mode
	TCCR1B |= (1 << WGM12);
	// set prescaler to 64 and starts PWM
	//TCCR1B |= (1 << CS11) | (1 << CS10);
	//TCCR1B |= (1 << CS12);

	/* Tachometer (Drehzahlmesser [for RPM]) */
	// -- TIMER 3 Setting to CTC --
	// set CTC mode (connect pins to OCR1x)
	TCCR3A |= ( (1 << COM3A0) );
	// set CTC Immediate Mode
	TCCR3B |= (1 << WGM32);
	// set prescaler to 64 and starts PWM
	//TCCR1B |= (1 << CS11) | (1 << CS10);
	//TCCR3B |= (1 << CS32);

}


void gauge_speedo_off (void)
{
	TCCR1B &= ~(1 << CS12);		// Disconnect Timer from clock
	PORTB &= ~(1 << PB5);		// Set Pin to low
}


void gauge_tacho_off (void)
{
	TCCR3B &= ~(1 << CS32);
	PORTE &= ~(1 << PE3);		// Set Pin to low
}


void gauge_speedo_on (void)
{
	TCCR1B |= (1 << CS12);		// Connect Timer1, set prescaler to ...
}


void gauge_tacho_on (void)
{
	TCCR3B |= (1 << CS32);		// Connect Timer3, set prescaler to ...
}



