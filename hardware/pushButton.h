/*
 * pushButton.h
 *
 */

#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_

#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stdint.h"

typedef struct {
	uint8_t states;	//holds the current and previous states of the button in binary form : the first bit is the current state, the second bit represents the previous state
	GPIO_TypeDef* port;
	uint32_t pin;
} pushButton_t;

void pushButton_init(pushButton_t *button);	//sets up the button with the internal pullup
void pushButton_updateStates(pushButton_t *button);	//reads the button input and update it's state
uint8_t pushButton_risingEdge(pushButton_t *button);	//returns true if a rising edge has been detected
uint8_t pushButton_fallingEdge(pushButton_t *button);	//returns true if a falling edge has been detected

#endif /* PUSHBUTTON_H_ */
