/*
 * rollodecks.c
 *
 */

#include "rollodecks.h"
#include "stm32f3xx_hal.h"
#include "ADC.h"

void rolloMUX_init(void) {
	GPIO_InitTypeDef GPIOStruct;

	ROLLOMUX_CLKENABLE();

	GPIOStruct.Pin = ROLLOMUX_PIN1 | ROLLOMUX_PIN2 | ROLLOMUX_PIN3;
	GPIOStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(ROLLOMUX_PORT, &GPIOStruct);

	//populate the rollo CV values with something different than 0 to avoid adsr blocking
	rolloCV_values[0] = 1;
	rolloCV_values[1] = 1;
	rolloCV_values[2] = 1;
	rolloCV_values[3] = 1;

	/*
	 * Init substate
	 */
	rollodecks_subState = SETMUX1;
	rolloMUX_set(0);
}

void rolloMUX_set(uint8_t position) {
	GPIOB->ODR &= ~(0x0007);	//the three last bits
	GPIOB->ODR |= (position & 0x0007);
}

void rollodecks_update(void) {
//	rolloCV_values[0] = ADC_getRolloCV();
	switch(rollodecks_subState) {
	case SETMUX1:
		rolloMUX_set(0);
		//start conversion
		break;
	case SETMUXREAD2:
		rolloCV_values[0] = ADC_getRolloCV();
		rolloMUX_set(1);
		break;
	case SETMUXREAD3:
		rolloCV_values[1] = ADC_getRolloCV();
		rolloMUX_set(2);
		break;
	case SETMUXREAD4:
		rolloCV_values[2] = ADC_getRolloCV();
		rolloMUX_set(3);
		break;
	case READ5:
		rolloCV_values[3] = ADC_getRolloCV();
		break;
	}
	rollodecks_subState = (++rollodecks_subState)%5;	//to loop back to the first substate
}
