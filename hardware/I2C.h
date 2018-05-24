/*
 * I2C.h
 *
 */

#ifndef I2C_H_
#define I2C_H_

/*
 * HAL include(s)
 */
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include <stdint.h>



/* I2C TIMING Register define when I2C clock source is SYSCLK
 * Fast as fuck
 */
#define I2C_TIMING      0x0000020B


/*
 * I2C timeout value
 */
#define I2C_TIMEOUT_VAL 10

/*
 * The actual handle to the I2C2 controller
 */
I2C_HandleTypeDef I2c2Handle;



/*
 * I2C controller configuration
 */
void i2c_Config(void);


#endif /* I2C_H_ */
