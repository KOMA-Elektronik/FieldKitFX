/*
 * buttonArray.c
 *
 */

#include "buttonArray.h"


void buttonArray_init(buttonArray_t* ba) {
	GPIO_InitTypeDef GPIOStruct;

	__HAL_RCC_GPIOC_CLK_ENABLE();

	//Load pin
	GPIOStruct.Pin = (ba->loadPinNumber);
	GPIOStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(ba->loadPort, &GPIOStruct);

	//Chip Enable pin
	GPIOStruct.Pin = (ba->chipEnablePinNumber);
	GPIOStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(ba->chipEnablePort, &GPIOStruct);

	//set the state registers
	ba->currentState = 0x0000;
	ba->previousState = 0x0000;

	//init the counter
	ba->longPressCounter = 0;

	//init the combination flag
	ba->activeCombination = NO_COMBINATION;

	//set the pins to the right levels
	HAL_GPIO_WritePin(ba->loadPort, ba->loadPinNumber, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ba->chipEnablePort, ba->chipEnablePinNumber, GPIO_PIN_SET);
}

void buttonArray_update(buttonArray_t* ba) {
	uint8_t temp[2];

	//shift the current state into the old one
	ba->previousState = ba->currentState;

	HAL_SPIEx_FlushRxFifo(ba->spiController);	//just to be sure

	//load pin low
	HAL_GPIO_WritePin(ba->loadPort, ba->loadPinNumber, GPIO_PIN_RESET);
	//and high
	HAL_GPIO_WritePin(ba->loadPort, ba->loadPinNumber, GPIO_PIN_SET);
	//put the CE pin low
	HAL_GPIO_WritePin(ba->chipEnablePort, ba->chipEnablePinNumber, GPIO_PIN_RESET);
	//read thru SPI
	HAL_SPI_Receive(ba->spiController, temp, 2, 1000);
	//put the CE pin high
	HAL_GPIO_WritePin(ba->chipEnablePort, ba->chipEnablePinNumber, GPIO_PIN_SET);

	ba->currentState = (temp[1] << 8) | temp[0];
}

void buttonArray_checkCombinations(buttonArray_t* ba){
	/*
		 * check for the calibration button combination
		 */
	if(buttonArray_isPressed(ba,0) && buttonArray_isPressed(ba,1) && buttonArray_isPressed(ba,9) && buttonArray_isPressed(ba,10)){
		if(++(ba->longPressCounter) > BUTTON_ARRAY_LONG_PRESS){
			ba->activeCombination = CALIBRATION_COMBINATION;
		}
	}else{
		ba->longPressCounter = 0;
		ba->activeCombination = NO_COMBINATION;
	}
}

void buttonArray_resetCombinations(buttonArray_t* ba){
	ba->longPressCounter = 0;
	ba->activeCombination = 0;
}

uint16_t buttonArray_activeCombination(buttonArray_t* ba){
	return ba->activeCombination;
}

uint8_t buttonArray_risingEdge(buttonArray_t* ba, uint8_t buttonIndex) {
	return (!(ba->previousState & (1 << buttonIndex))) && ((ba->currentState) & (1 << buttonIndex));
}
uint8_t buttonArray_fallingEdge(buttonArray_t* ba, uint8_t buttonIndex) {
	return (ba->previousState & (1 << buttonIndex)) && (!((ba->currentState) & (1 << buttonIndex)));
}

uint8_t buttonArray_isPressed(buttonArray_t* ba, uint8_t buttonIndex) {
	return (ba->previousState & (1 << buttonIndex)) && ((ba->currentState) & (1 << buttonIndex));
}
