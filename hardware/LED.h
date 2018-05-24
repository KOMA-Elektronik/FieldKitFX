/*
 * LED.h
 *
 */

#ifndef LED_H_
#define LED_H_

#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stdint.h"

/*
 * The LEDs used to indicate the state of the sequencer
 */

#define LED0_PORT GPIOE
#define LED0_PIN GPIO_PIN_2

#define LED1_PORT GPIOE
#define LED1_PIN GPIO_PIN_3

#define LED2_PORT GPIOE
#define LED2_PIN GPIO_PIN_4

#define LED3_PORT GPIOE
#define LED3_PIN GPIO_PIN_5

typedef struct {
	GPIO_TypeDef* port;
	uint32_t pin;
} LED_t;

/*
 * declare the structures for the LEDs used by the sequencer
 */
LED_t led0;
LED_t led1;
LED_t led2;
LED_t led3;

void LED_init(LED_t *led);	//sets up the LED
void LED_setHigh(LED_t *led);	//sets the LED high
void LED_setLow(LED_t *led);	//sets the LED low


#endif /* LED_H_ */
