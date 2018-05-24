/*
 * RGBLED.c
 *
 */
#include "RGBLED.h"


void RGBLED_SetColor(RGBLED_t *led) {
	LEDDriver_write3Val(led->driver, led->baseAddress, led->R, led->G, led->B);
}


void RGBLED_SetIntensity(RGBLED_t *led) {
	LEDDriver_write3Val(led->driver, led->baseAddress + 24, led->intensity, led->intensity, led->intensity);
}
