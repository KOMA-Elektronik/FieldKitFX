/*
 * LEDDriver.c
 */

#include "LEDDriver.h"

/*
 * Writes the specified data in the specified register
 * The last parameter is used to set or not the AutoIncrement Flag (AIF)
 */
void LEDDriver_writeReg(LEDDriver_t *driver, uint8_t regAddr, uint8_t data, uint8_t autoInc) {
	uint8_t temp[2];

	//if we want the autoIncrement flag to be set or not
	if(autoInc) {
		regAddr |= (1 << 7);
	}
	else {
		regAddr &= ~(1 << 7);
	}


	//Concatenate the regAddress and the data
	temp[0] = regAddr;
	temp[1] = data;

	//Send the data
	if(HAL_I2C_Master_Transmit(driver->i2cController, (uint16_t) ((driver->i2cAddr)<<1), temp, 2, I2C_TIMEOUT_VAL) != HAL_OK)
		__asm("bkpt");
}

/*
 * Writes the specified data in the specified register
 * The last parameter is used to set or not the AutoIncrement Flag (AIF)
 */
void LEDDriver_write3Val(LEDDriver_t *driver, uint8_t startAddr, uint8_t data1, uint8_t data2, uint8_t data3){
	uint8_t temp[4];

	//set the autoInc
	startAddr |= (1 << 7);



	//Concatenate the regAddress and the data
	temp[0] = startAddr;
	temp[1] = data1;
	temp[2] = data2;
	temp[3] = data3;

	//Send the data
	while(HAL_I2C_Master_Transmit(driver->i2cController, (uint16_t) ((driver->i2cAddr)<<1), temp, 4, I2C_TIMEOUT_VAL) != HAL_OK){
		;
	}
	while(HAL_I2C_GetState(driver->i2cController) != HAL_I2C_STATE_READY)
		;
}

/*
 * Writes the specified data in the specified register
 * The last parameter is used to set or not the AutoIncrement Flag (AIF)
 */
void LEDDriver_write1Val(LEDDriver_t *driver, uint8_t addr, uint8_t data){
	uint8_t temp[2] = {0};


	//Concatenate the regAddress and the data
	temp[0] = addr;
	temp[1] = data;

	//Send the data
	while(HAL_I2C_Master_Transmit(driver->i2cController, (uint16_t) ((driver->i2cAddr)<<1), temp, 2, I2C_TIMEOUT_VAL) != HAL_OK)
		;
}


/*
 * Initializes the LEDDriver to it's default behavior
 */
void LEDDriver_Init(LEDDriver_t *driver) {
	//Set the AI0 and AI1 bits correctly to allow autoIncrementation for the PWMx registers
	LEDDriver_writeReg(driver, MODE1, 0x28, 0);	//no autoInc
}
