 

#ifndef TIMER_H_
#define TIMER_H_

#include "config.h"

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "timer.h"

//#include "log.h"
/*!
 * Makro to calculate Ticks in ms
 * (ms / ticks opt. cast to uint32, if greater values)
 */
#define TICKS_TO_MS(ticks)	((ticks)*(TIMER_STEPS/8)/(1000/8))

/*!
 * Makro to calculate ms in Ticks
 * (ms / ticks opt. cast to uint32, if greater values)
 */
#define MS_TO_TICKS(ms)		((ms)*(1000/8)/(TIMER_STEPS/8))

/*!
 * Makro to calculate ms in Ticks
 * (ms / ticks opt. cast to uint32, if greater values)
 */
#define S_TO_TICKS(s)		MS_TO_TICKS(ms)*1000

#ifdef TIME_AVAILABLE
/*!
 * This function returnes the system time in parts of milliseconds.
 * @return	Milliseconds of system time
 */
uint16_t timer_get_ms(void);

/*!
 * Diese Funktion liefert den Sekundenanteil der Systemzeit zurueck.
 * @return Sekunden der Systemzeit
 */
uint16_t timer_get_s(void);

/*!
 * Returns seconds passed since old_s, old_ms
 * @param old_s		old value for the seconds
 * @param old_ms	old value for the milliseconds
 */
uint16_t timer_get_ms_since(uint16_t old_s, uint16_t old_ms);
#endif // TIME_AVAILABLE



/*! Union forr TickCount in 8, 16 and 32 Bit */
typedef union {
	uint32_t u32;		/*!< 32 Bit Integer */
	uint16_t u16;		/*!< 16 Bit Integer */
	uint8_t u8;		/*!< 8 Bit Integer */
} tickCount_t;
extern volatile tickCount_t tickCount;			/*!< one Tick every 176 us */

/*
 * Set system time back to 0
 */
static inline void timer_reset(void) {
	tickCount.u32 = 0;
}

#define TIMER_GET_TICKCOUNT_8  tickCount.u8		/*!< Zeit in 8 Bit */
#define TIMER_GET_TICKCOUNT_16 tickCount.u16	/*!< Zeit in 16 Bit */
#define TIMER_GET_TICKCOUNT_32 tickCount.u32	/*!< Zeit in 32 Bit */


// Values for TIMER_X_CLOCK are in ** Hz **

/*!
 * Frequency Timer 2 in Hz
 */
#define TIMER_2_CLOCK	5619	// Currently used for RC5

/*!
 * Microseconds passed since two Timer calls
 */
#define TIMER_STEPS 176

/*!
 * @brief				Checks if since the lase call min. ms milliseconds have passed
 * @param old_ticks		Pointer to a variable able to save a timestamp
 * @param ms			Time in ms that have to be passed, sothat True can be returned
 * @return				True or False
 *
 * This function actualizes the timestamp holding the old time to be compared and saves it automatically if
 * ms milliseconds have been passed.
 * To be used like this:
 * 
 * 	static uint32 old_time;
 * 	...
 * 	if (timer_ms_passed(&old_time, 50)) {
 * 			// is done every 50ms //
 * 	}
 * 	...
 */
#ifndef DOXYGEN
	static inline uint8_t __attribute__((always_inline)) timer_ms_passed(void * old_ticks, uint32_t ms) {
#else
	static inline uint8_t timer_ms_passed(void* old_ticks, uint32_t ms) {
#endif

	/* 8 Bit Version */
	if (MS_TO_TICKS(ms) < UINT8_MAX) {
		register uint8_t ticks = TIMER_GET_TICKCOUNT_8;
		if ((uint8_t)(ticks - *(uint8_t *)old_ticks) > MS_TO_TICKS(ms)) {
			*(uint8_t *)old_ticks = ticks;
			return TRUE;
		}
		return FALSE;

	/* 16 Bit Version */
	} else if (MS_TO_TICKS(ms) < UINT16_MAX) {
		register uint16_t ticks = TIMER_GET_TICKCOUNT_16;
		if ((uint16_t)(ticks - *(uint16_t *)old_ticks) > MS_TO_TICKS(ms)) {
			*(uint16_t *)old_ticks = ticks;
			return TRUE;
		}
		return FALSE;

	/* 32 Bit Version */
	} else {
		register uint32_t ticks = TIMER_GET_TICKCOUNT_32;
		if ((uint32_t)(ticks - *(uint32_t *)old_ticks) > MS_TO_TICKS(ms)) {
			*(uint32_t *)old_ticks = ticks;
			return TRUE;
		}
		return FALSE;
	}
}




/*!
 * Initializes Timer 0 and starts it
 */
void timer_2_init(void);

/*!
 * Measures the timelapse executing the __code 
 * and outputs it on the LOG or Display 
 * @param __code	Code to be measured
 */

/**
#define TIMER_MEASURE_TIME(__code) {			\
	uint32_t start = TIMER_GET_TICKCOUNT_32;	\
	{ __code; }					\
	uint32_t end = TIMER_GET_TICKCOUNT_32;		\
	uint16_t diff = end - start;			\
	LOG_DEBUG("%u Ticks", diff);			\
	display_cursor(4, 1);				\
	display_printf("%4u Ticks", diff);		\
}
**/
				
	

/*!
 * Wait 100 ms
 */
#define delay_100ms()	delay(100)

/*!
 * Waits for ms milliseconds
 * @param ms 	Number of milliseconds to wait
 */
void delay(uint16_t ms);


/*!
 * Delay loop using a 16-bit counter so up to 65536 iterations are possible. 
 * (The value 65536 would have to be passed as 0.) 
 * The loop executes four CPU cycles per iteration, not including the overhead 
 * the compiler requires to setup the counter register pair. 
 * Thus, at a CPU speed of 1 MHz, delays of up to about 262.1 milliseconds can be achieved.
 * @param __count	1/4 CPU-Cycles
 */
static inline void _delay_loop_2(uint16_t __count)
{
	__asm__ volatile (
		"1: sbiw %0,1" "\n\t"
		"brne 1b"
		: "=w" (__count)
		: "0" (__count)
	);
}







	
#endif /*TIMER_H_*/



