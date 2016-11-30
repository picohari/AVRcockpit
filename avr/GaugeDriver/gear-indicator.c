

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "timer.h"

#include "gear-indicator.h"
#include "font5x7.h"


unsigned char row_index = 0;		// Used for multiplexing the rows of the 5x7 dot matrix display
unsigned long glyph = 0;			// ASCII Code of the glyph to be displayed

unsigned long animate = 0;				// Counter for animation, scrolling effect
int cycle[7] = {0, 1, 2, 3, 4, 5, 6};	// Maps current row to a animated position


void gear_indicator_init(void)
{
	// Columns Pattern to OUTPUT (LED in one column, powered directly by PORTD)
	DDRD = 0xF8;
	PORTD = 0x00;

	// Rows Multiplexer to OUTPUT (ULN 2003)
	DDRC = 0x7F;
	PORTC = 0x00;
}


void gear_show(uint8_t glyphcode)
{
	switch (glyphcode)
	{
		case 0x30: glyph = '0'; break;
		case 0x31: glyph = '1'; break;
		case 0x32: glyph = '2'; break;
		case 0x33: glyph = '3'; break;
		case 0x34: glyph = '4'; break;
		case 0x35: glyph = '5'; break;
		case 0x36: glyph = '6'; break;
		case 0x37: glyph = '7'; break;
		case 0x38: glyph = '8'; break;
		case 0x39: glyph = '9'; break;  // R
		case 0x3A: glyph = ':'; break;  // N
		case 0x3B: glyph = ';'; break;  // blank
		case 0x3C: glyph = '<'; break;  // :)

		case 0x75: glyph = '='; break;	// UP
		case 0x64: glyph = '>'; break;	// DOWN

		case 0x61: glyph = '?'; break;  // :| FULL
		case 0x73: glyph = '@'; break;	// :| NEG

		case 0x71: glyph = 'A'; break;  // CLUTCH Open
		case 0x77: glyph = 'B'; break;	// CLUTCH Close

		case 0x6c: glyph = 'C'; break;	// L = Limiter

		// Default: 0x3b
		default: glyph = ';';			// Nothing!
	}

}


void rotate_left(int *array, int n)
{

	int temp = array[0]; 	// store first value

	for(int i = 0; i < n-1; i++) {
		array[i] = array[i+1]; 
	}
	array[n-1] = temp; 		// put that first value on then end
}


void rotate_right(int *array, int n)
{

	int temp = array[n-1]; 	// store last value

	for(int i = n-1; i > 0; i--) {
		array[i] = array[i-1]; 
	}
	array[0] = temp; 		// put that last value on then beginning
}


void display_glyph(unsigned char row, unsigned char glyph)
{
	uint8_t pattern;

	// Get pattern for each row from lookup-table: First ROW is 0x01 starting from TOP going DOWN
	pattern = pgm_read_byte(&font5x7[ (int)(glyph-48) ][ cycle[row] ]);
	
	// Delete old port settings
	PORTD &= ~( (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7) );
	PORTC &= ~( (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5) | (1 << PC6) );
	
	// Set new pattern to port
	PORTD |= ( 0xF8 & (pattern) << 3);

	// Shift ROW one bit DOWN (OFF by ONE as we use PD0 as RX in for UART)
	PORTC = (1 << row_index);

}


void gear_isr(void)
{

	// Refresh frequency... The Darlingtons are not very fast, so give some time to go down....
	if ( (animate % 10) == 0)
	{
		display_glyph(row_index, glyph);	// Call display function

		row_index++;						// Select next row next time
		row_index = row_index % 7;			// Stay in boundaries... 0b0000.0001 to 0b0100.0000
	}

	// Animate some symbols, mainly scrolling effect
	if ((animate % 300) == 0)
	{
		switch (glyph)		// Do this only for selected glyphs...
		{
			case '=': rotate_left(cycle, 7); break;		// We shift the pattern throug an array to get up/down animation
			case '>': rotate_right(cycle, 7); break;

			default:
			cycle[0] = 0;
			cycle[1] = 1;
			cycle[2] = 2;
			cycle[3] = 3;
			cycle[4] = 4;
			cycle[5] = 5;
			cycle[6] = 6;
		}
	}

	animate++;

}

