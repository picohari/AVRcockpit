

#include "config.h"



#define LIGHTS_ILLUMINATION	PA1
#define LIGHTS_HIGHBEAM		PA2	
#define LIGHTS_TURN_LEFT	PA4
#define LIGHTS_TURN_RIGHT	PA6



void lights_init(void);

void lights_illumination(uint8_t status);

void lights_highbeam(uint8_t status);

void lights_blinker(unsigned short timing, unsigned char blinker_mode);


