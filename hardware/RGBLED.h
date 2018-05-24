/*
 * RGBLED.h
 *
 */

#ifndef RGBLED_H_
#define RGBLED_H_

/*
 * Include(s)
 */
#include "LEDDriver.h"


/*
 * Definition of the RGBLED structure
 */
typedef struct {
	LEDDriver_t *driver;
	uint8_t baseAddress;

	//current flowing thru the RGB LED
	uint8_t intensity;

	// Color definition
	uint8_t R;
	uint8_t G;
	uint8_t B;

} RGBLED_t;


/*
 * Color and intensity update functions
 */
void RGBLED_SetColor(RGBLED_t *led);

void RGBLED_SetIntensity(RGBLED_t *led);

#endif /* RGBLED_H_ */
