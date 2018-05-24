/*
 * rolloSelector_switch.c
 *
 */

#include "rolloSelector_switch.h"

void rolloSelector_init() {
	GPIO_InitTypeDef GPIOStruct;

	ROLLOSELECTOR_CLK_ENABLE();
	GPIOStruct.Pin = ROLLOSELECTOR_PIN;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;
	GPIOStruct.Mode = GPIO_MODE_INPUT;

	HAL_GPIO_Init(ROLLOSELECTOR_PORT, &GPIOStruct);

	rolloSelector.current = ROLLO_START;
}

void rolloSelector_update() {
	rolloSelector.previous = rolloSelector.current;
	if(HAL_GPIO_ReadPin(ROLLOSELECTOR_PORT, ROLLOSELECTOR_PIN)) {
		rolloSelector.current = ROLLO_SEQ;
	}
	else {
		rolloSelector.current = ROLLO_ENV;
	}
}

rolloSelector_state rolloSelector_selectedRollo(rolloSelector_state rollo) {
	return (rolloSelector.current == rollo);
}

uint8_t rolloSelector_justSwitchedTo(rolloSelector_state rollo) {
	return ((rolloSelector.current == rollo) && (rolloSelector.previous != rollo));
}

