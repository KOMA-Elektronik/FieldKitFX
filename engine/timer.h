/*
 * timer.h
 *
 * This Timer is used to process sequencer and adsr at a fixed sample rate.
 * The corresponding functions are attached to the timer overflow isr.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "adsr.h"
#include "UI.h"
#include "sequencer.h"

TIM_HandleTypeDef SamplingTimer;

void Timer_init(void);


#endif /* TIMER_H_ */
