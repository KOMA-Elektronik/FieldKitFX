/*
 * FXSelector_switch.h
 *
 */

#ifndef FXSELECTOR_SWITCH_H_
#define FXSELECTOR_SWITCH_H_

#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include <stdint.h>


#define FXSELECTOR_CLK_ENABLE __HAL_RCC_GPIOD_CLK_ENABLE

#define FXSELECTOR_PORT GPIOD
#define FXSELECTOR_PIN GPIO_PIN_1

typedef enum {
	FX_FREQ_SHIFT = 0,
	FX_LOOPER,
	CALIBRATION
} FXSelector_state;

typedef struct {
	FXSelector_state previous, current;
} FXSelector_t;

FXSelector_t FXSelector;

void FXSelector_init();
void FXSelector_update();
void FXSelector_switchToCalibration();
FXSelector_state FXSelector_selectedFX();
uint8_t FXSelector_justSwitchedTo(FXSelector_state fx);

#endif /* FXSELECTOR_SWITCH_H_ */
