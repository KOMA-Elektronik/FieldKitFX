/*
 * loopButton.h
 *
 */

#ifndef LOOPBUTTON_H_
#define LOOPBUTTON_H_

#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include <stdint.h>

#include "RGBLED.h"

/*
 * UI related defines
 * Timings in milliseconds
 */
#define LONGPRESS_PERIOD_MIN 200

/*
 * GPIO Definition
 */
#define LOOPBUTTON_PORT_CLOCK __HAL_RCC_GPIOB_CLK_ENABLE()
#define LOOPBUTTON_PORT GPIOB
#define LOOPBUTTON_PIN GPIO_PIN_7
/*
 * RGB LED related definitions
 */
#define LOOPLED_BASEADDRESS 0x0a	//first LED
#define LOOPLED_NORMAL_INTENSITY 0x05
#define LOOPLED_HIGH_INTENSITY 0x20

#define LOOPLED_ARMED 0, 255, 0
#define LOOPLED_RECORD 255, 0, 0
#define LOOPLED_PLAYBACK 0, 0, 255
#define LOOPLED_OVERDUB	255, 0, 255
#define LOOPLED_ERASE	255, 255, 255

#define LOOPLED_OFF 0, 0, 0


typedef struct {
	uint8_t state;
	uint32_t lastUp_timestamp;
	uint32_t lastDown_timestamp;
	uint32_t current_timestamp;

	RGBLED_t LED;
} loopButton_t;

loopButton_t loopButton;

void loopButton_init(loopButton_t* lb, LEDDriver_t* driver);
void loopButton_updateStates(loopButton_t* lb);
uint8_t loopButton_risingEdge(loopButton_t* lb);
uint8_t loopButton_fallingEdge(loopButton_t* lb);
uint8_t loopButton_wasShortPress(loopButton_t* lb);
uint8_t loopButton_longPress(loopButton_t* lb);
void loopButton_changeColor(loopButton_t* lb, uint8_t R, uint8_t G, uint8_t B);
void loopButton_changeIntensity(loopButton_t* lb, uint8_t i);
void loopButton_updateLED(loopButton_t* lb);
uint8_t loopButton_isLow(loopButton_t* lb);
uint8_t loopButton_isHigh(loopButton_t* lb);
void loopButton_resetStates(loopButton_t* lb);

#endif /* LOOPBUTTON_H_ */
