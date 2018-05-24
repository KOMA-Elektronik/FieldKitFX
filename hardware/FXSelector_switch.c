/*
 * FXSelector_switch.c
 *
 */

#include "FXSelector_switch.h"

void FXSelector_init() {
	GPIO_InitTypeDef GPIOStruct;

	FXSELECTOR_CLK_ENABLE();
	GPIOStruct.Pin = FXSELECTOR_PIN;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;
	GPIOStruct.Mode = GPIO_MODE_INPUT;

	HAL_GPIO_Init(FXSELECTOR_PORT, &GPIOStruct);
}

void FXSelector_update() {
	FXSelector.previous = FXSelector.current;
	if(HAL_GPIO_ReadPin(FXSELECTOR_PORT, FXSELECTOR_PIN)) {
		FXSelector.current = FX_FREQ_SHIFT;
	}
	else {
		FXSelector.current = FX_LOOPER;
	}
}

void FXSelector_switchToCalibration(){
	FXSelector.previous = FXSelector.current;
	FXSelector.current = CALIBRATION;
}

FXSelector_state FXSelector_selectedFX() {
	return FXSelector.current;
}

uint8_t FXSelector_justSwitchedTo(FXSelector_state fx) {
	return ((FXSelector.current == fx) && (FXSelector.previous != fx));
}
