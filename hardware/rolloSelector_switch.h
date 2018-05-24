/*
 * rolloSelector_switch.h
 *
 */

#ifndef ROLLOSELECTOR_SWITCH_H_
#define ROLLOSELECTOR_SWITCH_H_

#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include <stdint.h>


#define ROLLOSELECTOR_CLK_ENABLE __HAL_RCC_GPIOD_CLK_ENABLE

#define ROLLOSELECTOR_PORT GPIOD
#define ROLLOSELECTOR_PIN GPIO_PIN_2

typedef enum {
	ROLLO_SEQ = 0,
	ROLLO_ENV,
	ROLLO_START
} rolloSelector_state;

typedef struct {
	rolloSelector_state previous, current;
} rolloSelector_t;

rolloSelector_t rolloSelector;

void rolloSelector_init();
void rolloSelector_update();
rolloSelector_state rolloSelector_selectedRollo(rolloSelector_state rollo);
uint8_t rolloSelector_justSwitchedTo(rolloSelector_state rollo);



#endif /* ROLLOSELECTOR_SWITCH_H_ */
