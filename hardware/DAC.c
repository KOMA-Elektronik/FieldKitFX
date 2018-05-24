/*
 * DAC.c
 */

#include "DAC.h"


void DAC_Init(void) {
	DAC_ChannelConfTypeDef sConfig;

	/*
	 * GPIO config
	 */


	__HAL_RCC_DAC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	DAC_GPIOConfig();


	/*
	 * DAC config
	 */

	hdac_rollo.Instance = DAC1;

	HAL_DAC_DeInit(&hdac_rollo);
	HAL_DAC_Init(&hdac_rollo);


	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

	if(HAL_DAC_ConfigChannel(&hdac_rollo, &sConfig, DAC1_CHANNEL_1) != HAL_OK){
		__asm("bkpt");
	}

	if(HAL_DAC_SetValue(&hdac_rollo, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, 0xF00) != HAL_OK){
		__asm("bkpt");
	}

}


void DAC_GPIOConfig(void) {
	GPIO_InitTypeDef GPIOStruct;
	GPIOStruct.Pin = GPIO_PIN_4;
	GPIOStruct.Mode = GPIO_MODE_ANALOG;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(GPIOA, &GPIOStruct);
}

void DAC_Start(void){
	if(HAL_DAC_Start(&hdac_rollo, DAC1_CHANNEL_1) != HAL_OK){
		__asm("bkpt");
	}
}

void DAC_setValue(uint16_t value){
	//limit value to 12bit
	value &= 0xFFF;
	if(HAL_DAC_SetValue(&hdac_rollo, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, value) != HAL_OK){
		__asm("bkpt");
	}
}
