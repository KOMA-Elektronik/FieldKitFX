/*
 * loopButton.c
 *
 */


#include "loopButton.h"


/*
 * Loop button specific colors definition
 */
const uint8_t loopButtonColors[3][3] = {
		{0, 0, 0},	//button OFF
		{255, 0, 0},	//red
		{255, 0, 255}	//purple
};


void loopButton_init(loopButton_t* lb, LEDDriver_t* driver) {

	LOOPBUTTON_PORT_CLOCK;
	GPIO_InitTypeDef GPIOStruct;

	GPIOStruct.Pin = LOOPBUTTON_PIN;
	GPIOStruct.Mode = GPIO_MODE_INPUT;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(LOOPBUTTON_PORT, &GPIOStruct);

	//init the timestamps
	lb->current_timestamp = 0;
	lb->lastUp_timestamp = 0;
	lb->lastDown_timestamp = 0;
	//and the state register
	lb->state = 0x00;

	//init the LED
	lb->LED.driver = driver;
	lb->LED.intensity = LOOPLED_NORMAL_INTENSITY;
	lb->LED.baseAddress = LOOPLED_BASEADDRESS;
	//turn the button OFF
	lb->LED.R = loopButtonColors[0][0];
	lb->LED.G = loopButtonColors[0][1];
	lb->LED.B = loopButtonColors[0][2];
	RGBLED_SetColor(&(lb->LED));
	RGBLED_SetIntensity(&(lb->LED));
}

void loopButton_updateStates(loopButton_t* lb) {
	//read the button and push the thing in the state register
	lb->state <<= 1;	//shift the states register to make place for the new state
	if(HAL_GPIO_ReadPin(LOOPBUTTON_PORT, LOOPBUTTON_PIN) == GPIO_PIN_SET) {
		lb->state |= 0x01;
	}

	//update the timestamp
	lb->current_timestamp = HAL_GetTick();

	//detect rising/falling edges
	loopButton_risingEdge(lb);
	loopButton_fallingEdge(lb);
}

uint8_t loopButton_risingEdge(loopButton_t* lb) {
	if( (lb->state & 0x01) && !((lb->state) & 0x02)) {
		//update the timestamps
		lb->lastUp_timestamp = lb->current_timestamp;
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t loopButton_fallingEdge(loopButton_t* lb) {
	if( !((lb->state) & 0x01) && ((lb->state) & 0x02)) {
		lb->lastDown_timestamp = lb->current_timestamp;
		return 1;
	}
	else {
		return 0;
	}
}

/*
 * If the user is "longpressing"
 */
uint8_t loopButton_longPress(loopButton_t* lb) {
	return (((lb->current_timestamp - lb->lastUp_timestamp) > LONGPRESS_PERIOD_MIN) && (lb->state & 0x01));
}

/*
 * was the last falling edge one of a long press ?
 */
uint8_t loopButton_wasLongPress(loopButton_t* lb) {
	return ((lb->current_timestamp - lb->lastUp_timestamp) > LONGPRESS_PERIOD_MIN);
}

void loopButton_changeColor(loopButton_t* lb, uint8_t R, uint8_t G, uint8_t B) {
	lb->LED.R = R;
	lb->LED.G = G;
	lb->LED.B = B;
	RGBLED_SetColor(&(lb->LED));
}


void loopButton_changeIntensity(loopButton_t* lb, uint8_t i) {
	lb->LED.intensity = i;
	RGBLED_SetIntensity(&(lb->LED));
}

void loopButton_resetStates(loopButton_t* lb) {
	lb->state = 0x00;
	lb->lastUp_timestamp = lb->current_timestamp;
	lb->lastDown_timestamp = lb->current_timestamp;
}

uint8_t loopButton_isLow(loopButton_t* lb) {
	return !(lb->state & 0x01);
}

uint8_t loopButton_isHigh(loopButton_t* lb) {
	return (lb->state & 0x01);
}


uint8_t loopButton_wasShortPress(loopButton_t* lb) {
	return (lb->lastDown_timestamp - lb->lastUp_timestamp) < LONGPRESS_PERIOD_MIN;
}
