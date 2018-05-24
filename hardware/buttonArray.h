/*
 * buttonArray.h
 *
 */

#ifndef BUTTONARRAY_H_
#define BUTTONARRAY_H_

#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"

#define BUTTON_ARRAY_LONG_PRESS 100

/*
 * Possible button combinations
 */

#define NO_COMBINATION 0
#define CALIBRATION_COMBINATION 1

typedef struct {
	SPI_HandleTypeDef* spiController;

	GPIO_TypeDef* loadPort;
	GPIO_TypeDef* chipEnablePort;
	uint16_t loadPinNumber, chipEnablePinNumber;

	uint16_t previousState, currentState;
	uint16_t longPressCounter;

	uint16_t activeCombination;
} buttonArray_t;

void buttonArray_init(buttonArray_t* ba);	//update the GPIOs, not the SPI controller
void buttonArray_update(buttonArray_t* ba);
void buttonArray_checkCombinations(buttonArray_t* ba);
void buttonArray_resetCombinations(buttonArray_t* ba);
uint16_t buttonArray_activeCombination(buttonArray_t* ba);
uint8_t buttonArray_risingEdge(buttonArray_t* ba, uint8_t buttonIndex);
uint8_t buttonArray_fallingEdge(buttonArray_t* ba, uint8_t buttonIndex);
/*
 * function that returns true when a button is pressed, meaning that previous and current state equal to 1
 */
uint8_t buttonArray_isPressed(buttonArray_t* ba, uint8_t buttonIndex);

#endif /* BUTTONARRAY_H_ */
