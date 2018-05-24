/*
 * CVRouter.c
 *
 */

#include "CVRouter.h"


/*
 * router.state[i]  <-- i=Destination
 * Destinations:
 * 1 -> AMOUNT
 * 2 -> CONTROL
 * 3 -> TRESHOLD
 * 4 -> TIME
 * 5 -> FBACK
 * 6 -> CUTOFF
 * 7 -> FBACK (SPRING)
 * 8 -> VCA1
 * 9 -> VCA2
 * 10 -> VCA3
 * 11 -> VCA4
 */

void CVRouter_init(CVRouter_t* router) {
	/*
	 * Init the GPIO load pin
	 */
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitTypeDef GPIOStruct;

	GPIOStruct.Pin = GPIO_PIN_6;
	GPIOStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOF, &GPIOStruct);

	/*
	 * The SPI related parameters are initialized in the routingMatrix Init routine
	 */

}

void CVRouter_setSwitches(CVRouter_t* router) {
	/*
	 * create the data to send from the state register
	 */
	uint8_t temp[7];


	temp[6] = (((router->state)[1] << 5) | ((router->state)[0] & 0x1F)); //dest 1 and 0
	temp[5] = (((router->state)[2] << 7) | (((router->state)[10] & 0x1F) << 2) | (((router->state)[1] & 0x1F) >> 3)); //dest 0 and 2
	temp[4] = (((router->state)[3] << 4) | (((router->state)[2] & 0x1F) >> 1));
	temp[3] = (((router->state)[5] << 6)| (((router->state)[4] & 0x1F) << 1) | (((router->state)[3] & 0x1F) >> 4));
	temp[2] = (((router->state)[6] << 3) | (((router->state)[5] & 0x1F) >> 2));
	temp[1] = (((router->state)[8] << 5) | ((router->state)[7] & 0x1F));
	temp[0] = ((((router->state)[9] & 0x1F) << 2) | (((router->state)[8] & 0x1F) >> 3)); //dest 0 and 2

	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_RESET);

	HAL_SPI_Transmit(router->spi_controller, temp, 7, 100);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_RESET);
}

void CVRouter_disableAllRoutings(CVRouter_t* router) {
	uint8_t i;
	for(i=0; i<11; i++) {
		(router->state)[i] = CV_SOURCE_REFERENCE;
	}
	CVRouter_setSwitches(router);
}

void CVRouter_setDestination(CVRouter_t* router, uint8_t src, uint8_t dst) {
	(router->state)[dst] = (1 << src);
	CVRouter_setSwitches(router);
}

void CVRouter_cycleSource(CVRouter_t* router, uint8_t dst) {
	switch((router->state)[dst]) {
	case CV_SOURCE_REFERENCE:
		(router->state)[dst] = CV_SOURCE_1;
		break;
	case CV_SOURCE_1:
		(router->state)[dst] = CV_SOURCE_2;
		break;
	case CV_SOURCE_2:
		(router->state)[dst] = CV_SOURCE_3;
		break;
	case CV_SOURCE_3:
		(router->state)[dst] = CV_SOURCE_4;
		break;
	case CV_SOURCE_4:
		(router->state)[dst] = CV_SOURCE_REFERENCE;
		break;
	}
}

uint8_t CVRouter_getSource(CVRouter_t* router, uint8_t dst) {
	uint8_t ret;

	switch((router->state)[dst]) {
	case CV_SOURCE_REFERENCE:
		ret = 0;
		break;
	case CV_SOURCE_1:
		ret = 1;
		break;
	case CV_SOURCE_2:
		ret = 2;
		break;
	case CV_SOURCE_3:
		ret = 3;
		break;
	case CV_SOURCE_4:
		ret = 4;
		break;
	default:
		ret = 0;
		break;
	}

	return ret;
}
