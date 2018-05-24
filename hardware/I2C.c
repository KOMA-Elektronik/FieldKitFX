/*
 * I2C.c
 *
 */

#include "I2C.h"


void i2c_Config(void) {

	/*
	 * Select a clock source for the I2C peripheral
	 */
	RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
	RCC_PeriphCLKInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_SYSCLK;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

	/*
	 * Initialize the I2C2 controller's registers
	 */
	__HAL_RCC_I2C2_CLK_ENABLE();

	I2c2Handle.Instance = I2C2;

	I2c2Handle.Init.Timing = I2C_TIMING;
	I2c2Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2c2Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2c2Handle.Init.OwnAddress1 = 0xff;	//not used
	I2c2Handle.Init.OwnAddress2 = 0xff;	//not used
	I2c2Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	I2c2Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;

	if(HAL_I2C_Init(&I2c2Handle) != HAL_OK) {
		/* Initialization Error */
		__asm("bkpt");
	}

	/*
	 * Finally initialize the GPIO pins of the I2C2 controller
	 */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
