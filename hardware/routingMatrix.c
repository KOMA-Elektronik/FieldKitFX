/*
 * routingMatrix.c
 *
 */


#include "routingMatrix.h"
#include "colors.h"

#include "debug.h"

void routingMatrix_init(routingMatrix_t* matrix) {
	/*
	 * --------- Init the SPI controller
	 */

	__HAL_RCC_SPI1_CLK_ENABLE();

	matrix->spiController.Instance = SPI1;
	matrix->spiController.Init.Mode = SPI_MODE_MASTER;		//Master mode
	matrix->spiController.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;	//32MHz/2 => 16.5Mbps
	matrix->spiController.Init.Direction = SPI_DIRECTION_2LINES;		//2ways communication
	matrix->spiController.Init.DataSize = SPI_DATASIZE_8BIT;		//8 bit transmission
	matrix->spiController.Init.FirstBit = SPI_FIRSTBIT_MSB;		//MSB first
	matrix->spiController.Init.CLKPolarity = SPI_POLARITY_LOW;		//clock idle low
	matrix->spiController.Init.CLKPhase	= SPI_PHASE_1EDGE;		//sampling on rising edge
	matrix->spiController.Init.NSS = SPI_NSS_SOFT;		//Software NSS
	matrix->spiController.Init.NSSPMode = SPI_NSS_PULSE_DISABLED;
	matrix->spiController.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;		//No CRC calculation
	matrix->spiController.Init.TIMode = SPI_TIMODE_DISABLE;

	HAL_SPI_Init(&(matrix->spiController));

	//And the pins that go with it
	GPIO_InitTypeDef GPIOStruct;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIOStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIOStruct.Mode = GPIO_MODE_AF_PP;
	GPIOStruct.Alternate = GPIO_AF5_SPI1;
	GPIOStruct.Pull = GPIO_PULLDOWN;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIOStruct);

	/*
	 * --------- Init the CV router
	 */
	matrix->router.spi_controller = &(matrix->spiController);

	CVRouter_init(&(matrix->router));

	/*
	 * --------- Init the buttons array
	 */
	matrix->ba.spiController = &(matrix->spiController);

	matrix->ba.loadPort = GPIOC;
	matrix->ba.loadPinNumber = GPIO_PIN_0;
	matrix->ba.chipEnablePort = GPIOC;
	matrix->ba.chipEnablePinNumber = GPIO_PIN_1;

	buttonArray_init(&(matrix->ba));

	/*
	 * --------- Init the RGB LEDs array
	 */
	/*
	 * I2C controller setup
	 * the MCU is the bus master
	 */
//	i2c_Config();
	U55.i2cAddr = U7_I2C_ADDRESS;
	U55.i2cController = &I2c2Handle;
	U54.i2cAddr = U6_I2C_ADDRESS;
	U54.i2cController = &I2c2Handle;

	LEDDriver_Init(&U54);
	LEDDriver_Init(&U55);

	/*
	 * RGB LED Array config
	 */
	matrix->la.size = RGBLEDARRAY_SIZE;
	RGBLEDArray_init(&(matrix->la), &U54, &U55);
}


void routingMatrix_reset(routingMatrix_t* matrix) {
	/*
	 * set all the modulation destinations to off
	 */
	CVRouter_disableAllRoutings(&(matrix->router));
	/*
	 * update the LEDs
	 */
	routingMatrix_syncLEDsToDestinations(matrix);
}


void routingMatrix_updateButtonStates(routingMatrix_t* matrix) {
	buttonArray_update(&(matrix->ba));
	buttonArray_checkCombinations(&(matrix->ba));
}

void routingMatrix_updateCVDestinations(routingMatrix_t* matrix) {
	uint8_t i;
	for(i=0; i<11; i++) {
		if(buttonArray_risingEdge(&(matrix->ba), i)) {
			CVRouter_cycleSource(&(matrix->router), i);
			//set the "has changed" bit (bit7)
			(matrix->router.state)[i] |= 0x80;

			matrix->switchUpdateFlag = 1;
			matrix->switchUpdateStartTime = HAL_GetTick();
		}
	}
	if((matrix->switchUpdateFlag == 1)&&(HAL_GetTick() - matrix->switchUpdateStartTime > MATRIX_SWITCHUPDATE_DELAY)) {
		CVRouter_setSwitches(&(matrix->router));
		matrix->switchUpdateFlag = 0;
	}
}

void routingMatrix_syncLEDsToDestinations(routingMatrix_t* matrix) {
	/*
	 * for each destination in the router's state register, associate the right color
	 */
	uint8_t i;
	for(i=0; i<11; i++) {
		if((matrix->router.state)[i] & 0x80) {	//check if this state has changed
			//reset the "has changed" bit
			(matrix->router.state)[i] &= ~(0x80);
			switch((matrix->router.state)[i]) {
			case CV_SOURCE_REFERENCE:
				RGBLEDArray_setLEDColor(&(matrix->la), i, colors[0][0], colors[0][1], colors[0][2]);
				break;
			case CV_SOURCE_1:
				RGBLEDArray_setLEDColor(&(matrix->la), i, colors[1][0], colors[1][1], colors[1][2]);
				break;
			case CV_SOURCE_2:
				RGBLEDArray_setLEDColor(&(matrix->la), i, colors[2][0], colors[2][1], colors[2][2]);
				break;
			case CV_SOURCE_3:
				RGBLEDArray_setLEDColor(&(matrix->la), i, colors[3][0], colors[3][1], colors[3][2]);
				break;
			case CV_SOURCE_4:
				RGBLEDArray_setLEDColor(&(matrix->la), i, colors[4][0], colors[4][1], colors[4][2]);
				break;
			}
		}
	}
}

void routingMatrix_forceSyncLEDsToDestinations(routingMatrix_t* matrix) {
	/*
	 * for each destination in the router's state register, associate the right color
	 */
	uint8_t i;
	for(i=0; i<11; i++) {
		switch((matrix->router.state)[i]) {
		case CV_SOURCE_REFERENCE:
			RGBLEDArray_setLEDColor(&(matrix->la), i, colors[0][0], colors[0][1], colors[0][2]);
			break;
		case CV_SOURCE_1:
			RGBLEDArray_setLEDColor(&(matrix->la), i, colors[1][0], colors[1][1], colors[1][2]);
			break;
		case CV_SOURCE_2:
			RGBLEDArray_setLEDColor(&(matrix->la), i, colors[2][0], colors[2][1], colors[2][2]);
			break;
		case CV_SOURCE_3:
			RGBLEDArray_setLEDColor(&(matrix->la), i, colors[3][0], colors[3][1], colors[3][2]);
			break;
		case CV_SOURCE_4:
			RGBLEDArray_setLEDColor(&(matrix->la), i, colors[4][0], colors[4][1], colors[4][2]);
			break;
		}
	}
}
