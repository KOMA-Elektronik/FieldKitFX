/*
 * Gate.c
 *
 */

#include "Gate.h"


void Gate_init(Gate_t* gate, uint16_t pin, GPIO_TypeDef * port){

	gate->pin = pin;
	gate->port = port;

	GPIO_InitTypeDef GPIOStruct;

	GPIOStruct.Pin = gate->pin;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Mode = GPIO_MODE_INPUT;

	HAL_GPIO_Init(gate->port, &GPIOStruct);

	gate->current_state = 0;
	gate->previous_state = 0;
}

void Gate_update(Gate_t* gate){
	gate->previous_state = gate->current_state;
	gate->current_state = HAL_GPIO_ReadPin(gate->port, gate->pin);
}


uint32_t Gate_isHigh(Gate_t* gate){
	return HAL_GPIO_ReadPin(gate->port, gate->pin);
}

uint32_t Gate_isLow(Gate_t* gate){
	return (!(HAL_GPIO_ReadPin(gate->port, gate->pin)) & 1);
}

uint32_t Gate_transitionToHigh(Gate_t* gate){
	if((gate->current_state == 1)&&(gate->previous_state == 0)){
		return 1;
	}else{
		return 0;
	}
}

uint32_t Gate_transitionToLow(Gate_t* gate){
	if((gate->current_state == 0)&&(gate->previous_state == 1)){
		return 1;
	}else{
		return 0;
	}
}

