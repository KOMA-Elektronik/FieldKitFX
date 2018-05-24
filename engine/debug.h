/*
 * debug.h
 *
 * Some functions to utilize a custom debug port.
 * The pins can be probed directly on the chip.
 * PIN E 12 corresponds to PIN 43
 * PIN E 13 corresponds to PIN 44
 * PIN E 14 corresponds to PIN 45
 * PIN E 15 corresponds to PIN 46
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"

#define DEBUG_1_PIN GPIO_PIN_12
#define DEBUG_2_PIN GPIO_PIN_13
#define DEBUG_3_PIN GPIO_PIN_14
#define DEBUG_4_PIN GPIO_PIN_15
#define DEBUG_PORT GPIOE

#define DEBUG_CLK_ENABLE __HAL_RCC_GPIOE_CLK_ENABLE

void debug_init(void);

void debug_setPIN(uint16_t pin);

void debug_resetPIN(uint16_t pin);

void debug_togglePIN(uint16_t pin);

#endif /* DEBUG_H_ */
