/*
 * debug.c
 *
 */

#include "debug.h"

void debug_init(void){
	GPIO_InitTypeDef GPIOStruct;

	DEBUG_CLK_ENABLE();

	GPIOStruct.Pin = DEBUG_1_PIN | DEBUG_2_PIN | DEBUG_3_PIN | DEBUG_4_PIN;
	GPIOStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(DEBUG_PORT, &GPIOStruct);
}

void debug_setPIN(uint16_t pin){
	DEBUG_PORT->ODR |= pin;
}

void debug_resetPIN(uint16_t pin){
	DEBUG_PORT->ODR &= ~(pin);
}

void debug_togglePIN(uint16_t pin){
	DEBUG_PORT->ODR ^= pin;
}
