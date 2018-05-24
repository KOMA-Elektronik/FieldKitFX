/*
 * Gate.h
 *
 */

#ifndef GATE_H_
#define GATE_H_

#include <stdint.h>
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"

#define GATE_IN_PIN GPIO_PIN_2
#define GATE_IN_PORT GPIOC
#define GATE_IN_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE()

typedef struct{
	uint8_t current_state;
	uint8_t previous_state;
	uint16_t pin;
	GPIO_TypeDef * port;
} Gate_t;

Gate_t gate;

/*
 * This function inits the hardware part and assigns it to a gate structure
 */
void Gate_init(Gate_t* gate, uint16_t pin, GPIO_TypeDef * port);

/*
 * This function reads the input the input and updates the internal states
 */
void Gate_update(Gate_t* gate);

/*
 * Those functions return the current state of the Gate Input
 */
uint32_t Gate_isHigh(Gate_t* gate);
uint32_t Gate_isLow(Gate_t* gate);

/*
 * Those function return transitions of the Gate Input
 */
uint32_t Gate_transitionToHigh(Gate_t* gate);
uint32_t Gate_transitionToLow(Gate_t* gate);


#endif /* GATE_H_ */
