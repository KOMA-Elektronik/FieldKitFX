/*
 * main.c
 *
 */

#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"


#include "main.h"
#include "engine.h"
#include "codec.h"
#include "ADC.h"
#include "frequencyShifter.h"
#include "UI.h"
#include "memory.h"
#include "looper.h"
#include "sequencer.h"
#include "magnitudeTracker.h"
#include "Gate.h"
#include "timer.h"


int main(void)
{
	/*
	 * This function initializes the HAL :
	 * configures the Flash prefetch, time base source, NVIC and Low level hardware
	 * This function is called at the beginning of program after reset and before the clock configuration
	 */
	HAL_Init();

	/*
	 * Configures the clock distribution system, turns the external crystal on, initializes the peripheral clock prescalers
	 * end sets the SysTick to increment every millisecond
	 */
	systemClock_Config_HSE();

	/*============================================================================================================*/
	/*
	 * ADC config
	 */
	ADC_Init();
	ADC_Start();
	/*
	 * I2C controller setup
	 * the MCU is the bus master
	 */
	i2c_Config();

	/*
	 * start audio engine
	 */
	engine_init();	//sets up the buffer system

	/*
	 * set up the codec
	 */
	codec_ConfigGPIO();
	codec_ConfigAudioInterface();

	/*
	 * set up a frequency shifter instance
	 */
	frequencyShifter_t FS;
	frequencyShifter_init(&FS);
	frequencyShifter_setSideBandXfade(&FS, 1.0);	//only output the USB

	/*
	 * set a initial shift amount (500Hz)
	 */
	frequencyShifter_setShiftAmt(&FS, 500);

	/*
	 * sample to process
	 */
	int16_t sample;


	/*============================================================================================================*/
	/*
	 * External memory Init
	 */
	Memory_initSPI();
	RAMChip_t RAMArray;
	/*
	 * Initializes the CS pin and sets it idle
	 */
	RAM_init(&RAMArray);

	/*
	 * Looper init
	 */
	looper.memory = &RAMArray;
	looper_init(&looper);

	/*============================================================================================================*/
	/*
	 * UI Init
	 */
	UI_init();

	routingMatrix_forceSyncLEDsToDestinations(&CVmatrix);

	GATE_IN_CLK_ENABLE;
	Gate_init(&gate, GATE_IN_PIN, GATE_IN_PORT);

	/*
	 * and start the codec
	 */
	codec_ConfigCodec_48K();
	codec_start();

	DAC_Init();
	DAC_Start();

	sequencer_Init();
	ADSR_init(&rollo_env);
	Timer_init();

	/*============================================================================================================*/
	/*
	 * Main loop
	 */

	uint32_t eventCounter=0;

	for(;;) {
		if(FXSelector_selectedFX() == FX_FREQ_SHIFT) {
			/*
			 * Frequency shifter
			 */
			if((!audio_out_buffer_isFull(&user_audio_out_buffer)) && (!audio_in_buffer_isEmpty(&user_audio_in_buffer))) {
				/*
				 * Process a sample with the frequency shifter
				 */
				audio_in_buffer_pop(&user_audio_in_buffer, &sample);

				frequencyShifter_processSample(&FS, &sample);

				audio_out_buffer_push(&user_audio_out_buffer, sample);
			}
			else {
				if(eventCounter & (1 << 0)){
					frequencyShifter_updateCVs(&FS);
				}else{
					UI_render();
				}
				eventCounter++;
			}
		}
		else if(FXSelector_selectedFX() == FX_LOOPER){
			/*
			 * Looper
			 */
			if(audio_in_buffer_isFull(&user_audio_in_buffer)) {
				//Looper state machine
				looper_process(&looper);
				//apply the looper effects
				/*
				 * Bitcrusher
				 */
				looper_process_bitcrush(user_audio_out_buffer.buffer, user_audio_out_buffer.buffer, ADC_getMixedCV1());
				/*
				 * SRR
				 */
				looper_process_sampleRateReducer(user_audio_out_buffer.buffer, user_audio_out_buffer.buffer, ADC_getMixedCV2());
			}
			else {
				UI_render();
			}
		}else{//in calibration mode
			if(FXSelector_justSwitchedTo(CALIBRATION)){
				UI_initCalibration();
				FXSelector_switchToCalibration();
			}
			if((!audio_out_buffer_isFull(&user_audio_out_buffer)) && (!audio_in_buffer_isEmpty(&user_audio_in_buffer))) {
				//we want to feed trough the audio
				audio_in_buffer_pop(&user_audio_in_buffer, &sample);
				audio_out_buffer_push(&user_audio_out_buffer, sample);
				//and we want to track the maximum amplitude
				magnitudeTracker_processSample(sample);
			}else{
				if(eventCounter & (1 << 0)){
					UI_renderCalibration();
				}else{
					codec_setInputGain(CODEC_REG_LEFT_LINE_IN, (ADC_getMixedCV2() >> 7));
				}
				eventCounter++;
			}
		}

	}
}

void systemClock_Config_HSE(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		__asm("bkpt");
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		__asm("bkpt");
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 1);

	/* GPIO Ports Clock Enable */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*Configure GPIO pin : PC9 */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF6_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
