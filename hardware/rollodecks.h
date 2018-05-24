/*
 * rollodecks.h
 *
 */

#ifndef ROLLODECKS_H_
#define ROLLODECKS_H_

#include <stdint.h>

#define ROLLOMUX_PIN1 GPIO_PIN_0
#define ROLLOMUX_PIN2 GPIO_PIN_1
#define ROLLOMUX_PIN3 GPIO_PIN_2
#define ROLLOMUX_PORT GPIOB
#define ROLLOMUX_CLKENABLE	__HAL_RCC_GPIOB_CLK_ENABLE

typedef enum {
	SETMUX1 = 0,	//only set the MUXs and start ADC
	SETMUXREAD2,	//read ADC, set MUXs and start new conversion
	SETMUXREAD3,	//read ADC, set MUXs and start new conversion
	SETMUXREAD4,	//read ADC, set MUXs and start new conversion
	READ5			//only read the ADC
} rollodecks_subState_t;


rollodecks_subState_t rollodecks_subState;
uint16_t rolloCV_values[4];

void rolloMUX_init(void);
void rolloMUX_set(uint8_t position);
void rollodecks_update(void);

#endif /* ROLLODECKS_H_ */
