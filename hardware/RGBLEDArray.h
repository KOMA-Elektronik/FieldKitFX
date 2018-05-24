/*
 * RGBLEDArray.h
 *
 */

#ifndef RGBLEDARRAY_H_
#define RGBLEDARRAY_H_

/*
 * Includes
 */
#include "RGBLED.h"


/*
 * Defines of all the important RGB related addresses and stuff
 */
#define U6_I2C_ADDRESS 0x0d
#define U7_I2C_ADDRESS 0x33

#define CV_RGB_LED_INTENSITY 0x04

/*
 * LED Array related defines
 */
#define RGBLEDARRAY_SIZE 11

typedef struct {
	RGBLED_t array[RGBLEDARRAY_SIZE];
	uint8_t size;
}RGBLEDArray_t;

/*
 * RGBLEDArray functions
 */
void RGBLEDArray_init(RGBLEDArray_t* LEDArray, LEDDriver_t *driver1, LEDDriver_t *driver2);
void RGBLEDArray_setGlobalColor(RGBLEDArray_t* LEDArray, uint8_t _R, uint8_t _G, uint8_t _B);
void RGBLEDArray_setLEDColor(RGBLEDArray_t* LEDArray, uint8_t LEDIndex, uint8_t _R, uint8_t _G, uint8_t _B);
void RGBLEDArray_setGlobalIntensity(RGBLEDArray_t* LEDArray, uint8_t intensity);
void RGBLEDArray_setIntensity(RGBLEDArray_t* LEDArray, uint8_t LEDIndex, uint8_t intensity);


#endif /* RGBLEDARRAY_H_ */
