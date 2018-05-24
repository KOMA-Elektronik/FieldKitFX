/*
 * RGBLEDArray.c
 *
 */

#include "RGBLEDArray.h"

const uint8_t LEDBaseAddrLUT[RGBLEDARRAY_SIZE] = {0x0e, 0x12, 0x1f, 0x1b, 0x17, 0x0a, 0x0e, 0x12, 0x17, 0x1b, 0x1f};



void RGBLEDArray_init(RGBLEDArray_t* LEDArray, LEDDriver_t *driver1, LEDDriver_t *driver2) {
	uint8_t i;

	for(i=0; i < LEDArray->size; i++) {
		/*
		 * Init the RGBLED struct
		 */
		if(i < 5) {	//LEDs 1 to 6 are attached to the first LED driver (U54)
			(LEDArray->array)[i].driver = driver1;
		}
		else {
			(LEDArray->array)[i].driver = driver2;
		}

		(LEDArray->array)[i].baseAddress = LEDBaseAddrLUT[i];

		(LEDArray->array)[i].R = 0;
		(LEDArray->array)[i].G = 0;
		(LEDArray->array)[i].B = 0;
		(LEDArray->array)[i].intensity = CV_RGB_LED_INTENSITY;
		RGBLED_SetIntensity(&((LEDArray->array)[i]));
		RGBLED_SetColor(&((LEDArray->array)[i]));
	}
}

void RGBLEDArray_setGlobalColor(RGBLEDArray_t* LEDArray, uint8_t _R, uint8_t _G, uint8_t _B) {
	uint8_t i;

	for(i=0; i < LEDArray->size; i++) {
		(LEDArray->array)[i].R = _R;
		(LEDArray->array)[i].G = _G;
		(LEDArray->array)[i].B = _B;
		RGBLED_SetColor(&((LEDArray->array)[i]));
	}
}

void RGBLEDArray_setLEDColor(RGBLEDArray_t* LEDArray, uint8_t LEDIndex, uint8_t _R, uint8_t _G, uint8_t _B) {
	(LEDArray->array)[LEDIndex].R = _R;
	(LEDArray->array)[LEDIndex].G = _G;
	(LEDArray->array)[LEDIndex].B = _B;
	RGBLED_SetColor(&((LEDArray->array)[LEDIndex]));
}

void RGBLEDArray_setGlobalIntensity(RGBLEDArray_t* LEDArray, uint8_t intensity){
	for(uint8_t i=0; i < LEDArray->size; i++) {
		(LEDArray->array)[i].intensity = intensity;
		RGBLED_SetIntensity(&((LEDArray->array)[i]));
	}
}

void RGBLEDArray_setIntensity(RGBLEDArray_t* LEDArray, uint8_t LEDIndex, uint8_t intensity){
	(LEDArray->array)[LEDIndex].intensity = intensity;
	RGBLED_SetIntensity(&((LEDArray->array)[LEDIndex]));
}
