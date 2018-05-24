/*
 * timer.c
 *
 */

#include "timer.h"

void Timer_init(void){
	TIM_ClockConfigTypeDef clockSourceStruct;

	__HAL_RCC_TIM4_CLK_ENABLE();

	SamplingTimer.Instance = TIM4;
	HAL_TIM_Base_DeInit(&SamplingTimer);
	SamplingTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	SamplingTimer.Init.Period = 9;	//Effective IT rate : 10KHz
	SamplingTimer.Init.Prescaler = 719;	//100kHz update rate
	SamplingTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	SamplingTimer.Init.RepetitionCounter = 0;

	clockSourceStruct.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&SamplingTimer, &clockSourceStruct);

	if(HAL_TIM_Base_Init(&SamplingTimer) != HAL_OK) {
		__asm("bkpt");
	}

	/*
	 * Start the counter
	 */
	HAL_TIM_Base_Start_IT(&SamplingTimer);
	HAL_NVIC_SetPriority(TIM4_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&SamplingTimer);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4) {
		//only do stuff when the envelope is selected
		if(rolloSelector_selectedRollo(ROLLO_ENV)){
			ADSR_processSample(&rollo_env);
		}else if(rolloSelector_selectedRollo(ROLLO_SEQ)){
			sequencer_watch();
		}
	}
}
