

#include "config.h"


extern unsigned char row_index;		// Used for multiplexing the rows of the 5x7 dot matrix display
extern unsigned long glyph;		// ASCII Code of the glyph to be displayed

extern unsigned long animate;		// Counter for animation, scrolling effect
extern int cycle[7];			// Maps current row to a animated position


void gear_indicator_init(void);

void rotate_left(int *array, int n);

void rotate_right(int *array, int n);

void display_glyph(unsigned char row, unsigned char glyph);

void gear_show(uint8_t glyphcode);

void gear_isr(void);


