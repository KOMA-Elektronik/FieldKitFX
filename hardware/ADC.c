/*
 * ADC.c
 *
 */


#include "ADC.h"
#include "debug.h"

void ADC_Init(void) {

	ADC_ChannelConfTypeDef sConfig;

	/*
	 * GPIO config
	 */


	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	ADC_GPIOConfig();

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();


	/*
	 * ADC config
	 */

	hadc_cv.Instance = ADC_INSTANCE;

	HAL_ADC_DeInit(&hadc_cv);

	hadc_cv.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;	//72/4
#if ADC_RESOLUTION_BIT == 12
	hadc_cv.Init.Resolution = ADC_RESOLUTION_12B;
#elif ADC_RESOLUTION_BIT == 10
	hadc_cv.Init.Resolution = ADC_RESOLUTION_10B;
#else
#error "undefined ADC bitdepth"
#endif

	hadc_cv.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc_cv.Init.ContinuousConvMode = ENABLE;
	hadc_cv.Init.DiscontinuousConvMode = DISABLE;
	hadc_cv.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc_cv.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc_cv.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc_cv.Init.NbrOfConversion = NB_CV_IN;
	hadc_cv.Init.DMAContinuousRequests = ENABLE;
	hadc_cv.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc_cv.Init.LowPowerAutoWait = DISABLE;
	hadc_cv.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;


	if (HAL_ADC_Init(&hadc_cv) != HAL_OK)
	{
		__asm("bkpt");
	}

	/*
	 * ADC calibration
	 */
	HAL_ADC_Stop(&hadc_cv);
	HAL_ADCEx_Calibration_Start(&hadc_cv, ADC_SINGLE_ENDED);

	/*
	 * Configure Regular Channel
	 */

	//Mixed CV1
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.SamplingTime = ADC_SAMPLETIME_19CYCLES_5;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc_cv, &sConfig) != HAL_OK)
	{
		__asm("bkpt");
	}


	//Mixed CV2
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc_cv, &sConfig) != HAL_OK)
	{
		__asm("bkpt");
	}

	//Rollo CV
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&hadc_cv, &sConfig) != HAL_OK)
	{
		__asm("bkpt");
	}

	//Threshold Pot
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_4;
	if (HAL_ADC_ConfigChannel(&hadc_cv, &sConfig) != HAL_OK)
	{
		__asm("bkpt");
	}

	//Threshold CV
	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = ADC_REGULAR_RANK_5;
	if (HAL_ADC_ConfigChannel(&hadc_cv, &sConfig) != HAL_OK)
	{
		__asm("bkpt");
	}
	/* Peripheral DMA init*/

	hdma_cv.Instance = DMA1_Channel1;
	hdma_cv.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_cv.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_cv.Init.MemInc = DMA_MINC_ENABLE;
	hdma_cv.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_cv.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_cv.Init.Mode = DMA_CIRCULAR;
	hdma_cv.Init.Priority = DMA_PRIORITY_MEDIUM;
	if (HAL_DMA_Init(&hdma_cv) != HAL_OK) {
		__asm("bkpt");
	}

	__HAL_LINKDMA(&hadc_cv,DMA_Handle,hdma_cv);
}


void ADC_GPIOConfig(void) {
	GPIO_InitTypeDef GPIOStruct;
	GPIOStruct.Pin = GPIO_PIN_0;
	GPIOStruct.Mode = GPIO_MODE_ANALOG;
	GPIOStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &GPIOStruct);

	GPIOStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &GPIOStruct);

	GPIOStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOA, &GPIOStruct);

	GPIOStruct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &GPIOStruct);

	GPIOStruct.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOF, &GPIOStruct);
}

void ADC_Start(void) {
	HAL_ADC_Start_DMA(&hadc_cv, cv_data, NB_CV_IN);
	hadc_cv.DMA_Handle->XferCpltCallback = ADC_Xfr_cplt;
	hadc_cv.DMA_Handle->XferHalfCpltCallback = ADC_Xfr_Half_cplt;
}

void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_cv);	//no need to implement callbacks since we will not check the XferCplt and halfCplt
}

void ADC_Xfr_Half_cplt(DMA_HandleTypeDef * hdma){

}

void ADC_Xfr_cplt(DMA_HandleTypeDef * hdma){

}

uint32_t ADC_getMixedCV1(void) {
	return (ADC_RESOLUTION_DEZ - cv_data[0]);
}

uint32_t ADC_getMixedCV2(void) {
	return (ADC_RESOLUTION_DEZ - cv_data[1]);
}

uint32_t ADC_getRolloCV(void) {
	return (ADC_RESOLUTION_DEZ - cv_data[2]);
}

uint32_t ADC_getThresholdPot(void) {
	return (ADC_RESOLUTION_DEZ - cv_data[3]);
}

uint32_t ADC_getThresholdCV(void) {
	return (ADC_RESOLUTION_DEZ - cv_data[4]);
}
