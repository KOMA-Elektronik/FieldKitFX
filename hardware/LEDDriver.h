/*
 * LEDDriver.h
 *
 */

#ifndef LEDDRIVER_H_
#define LEDDRIVER_H_

/*
 * Include(s)
 */
#include "I2C.h"
#include "PCA9956B_RegMap.h"


/*
 * Definition of the LEDDriver structure
 */
typedef struct {
	I2C_HandleTypeDef *i2cController;
	uint8_t i2cAddr;
} LEDDriver_t;



/*
 * Initializes the LEDDriver to it's default behavior
 */
void LEDDriver_Init(LEDDriver_t *driver);

/*
 * writes <data> in the specified register of the specified PCA9956B (slave address identified)
 */
void LEDDriver_writeReg(LEDDriver_t *driver, uint8_t regAddr, uint8_t data, uint8_t autoInc);

void LEDDriver_write3Val(LEDDriver_t *driver, uint8_t startAddr, uint8_t data1, uint8_t data2, uint8_t data3);

void LEDDriver_write1Val(LEDDriver_t *driver, uint8_t addr, uint8_t data);

#endif /* LEDDRIVER_H_ */
