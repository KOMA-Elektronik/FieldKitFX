/*
 * LED.c
 *
 */

#include "LED.h"

void LED_init(LED_t *led){	//sets up the LED
	GPIO_InitTypeDef GPIOInitStruct;

	GPIOInitStruct.Pin = led->pin;
	GPIOInitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOInitStruct.Speed = GPIO_SPEED_MEDIUM;
	GPIOInitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(led->port, &GPIOInitStruct);
}


void LED_setHigh(LED_t *led){
	//sets the LED high
	HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_SET);
}


void LED_setLow(LED_t *led){
	//sets the LED low
	HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_RESET);
}
