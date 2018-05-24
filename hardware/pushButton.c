/*
 * pushButton.c
 *
 */

#include "../hardware/pushButton.h"

void pushButton_init(pushButton_t *button) {	//sets up the button with the internal pullup (high at idle stage)
	GPIO_InitTypeDef GPIOInitStruct;

	GPIOInitStruct.Pin = button->pin;
	GPIOInitStruct.Mode = GPIO_MODE_INPUT;
	GPIOInitStruct.Speed = GPIO_SPEED_MEDIUM;
	GPIOInitStruct.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(button->port, &GPIOInitStruct);

	//init the states variable
	button->states = HAL_GPIO_ReadPin(button->port, button->pin);
}


void pushButton_updateStates(pushButton_t *button) {	//reads the button input and update it's state
	button->states <<= 1;	//shift the states register to make place for the new state
	button->states |= HAL_GPIO_ReadPin(button->port, button->pin);	//place the newly read state in the register
}


uint8_t pushButton_risingEdge(pushButton_t *button) {	//returns true if a rising edge has been detected
	return (!(button->states & (1U)) && (button->states & (1 << 1)));	//if the current state is High and the previous one is Low (have to take the pullup in account)
}

uint8_t pushButton_fallingEdge(pushButton_t *button) {	//returns true if a falling edge has been detected
	return ((button->states & (1U)) && !(button->states & (1 << 1)));	//if the current state is Low and the previous one is High (have to take the pullup in account)
}
