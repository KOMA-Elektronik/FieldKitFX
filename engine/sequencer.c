/*
 * sequencer.c
 *
 */

#include "sequencer.h"
#include "debug.h"



void sequencer_Init(void){
	//init the GPIO CLK of the LED port
	__HAL_RCC_GPIOE_CLK_ENABLE();

	//assign and init all the LEDs
	led0.port = LED0_PORT;
	led0.pin = LED0_PIN;

	led1.port = LED1_PORT;
	led1.pin = LED1_PIN;

	led2.port = LED2_PORT;
	led2.pin = LED2_PIN;

	led3.port = LED3_PORT;
	led3.pin = LED3_PIN;

	LED_init(&led0);
	LED_init(&led1);
	LED_init(&led2);
	LED_init(&led3);

	LED_setLow(&led0);
	LED_setLow(&led1);
	LED_setLow(&led2);
	LED_setLow(&led3);

	//populate the  step array structures with their led and cv

	steps[0].step_led = &led0;
	steps[0].step_cv = &rolloCV_values[0];

	steps[1].step_led = &led1;
	steps[1].step_cv = &rolloCV_values[1];

	steps[2].step_led = &led2;
	steps[2].step_cv = &rolloCV_values[2];

	steps[3].step_led = &led3;
	steps[3].step_cv = &rolloCV_values[3];

	sequencer_setStep(0);

	movingAverageFilter_init(&sequencer_threshold_ma, sequencer_threshold_ma_array, SEQUENCER_THRSH_MA_LENGTH);
	movingAverageFilter_init(&sequencer_thresholdCV_ma, sequencer_thresholdCV_ma_array, SEQUENCER_THRSHCV_MA_LENGTH);

	sequencer_index = 0;
	sequencer_timepassed = 0;
	sequencer_mode = GATE;

	sequencer_processCV(ADC_getThresholdPot() >> 2);
}

void sequencer_watch(void){
	sequencer_timepassed++;
	if(sequencer_mode == POT){
		if(sequencer_timepassed >= sequencer_period){
			sequencer_setNextStep();
			sequencer_timepassed = 0;
		}
	}else if(sequencer_mode == GATE){
		Gate_update(&gate);
		if(Gate_transitionToHigh(&gate)){
			sequencer_setNextStep();
		}
	}else if(sequencer_mode == CV){
		if(sequencer_timepassed == 10){//that's done to reduce the "sampling frequency"
			sequencer_timepassed = 0;
			sequencer_envelopestate.previous = sequencer_envelopestate.current;
			//using only the postive halfwave for envelope following
			sequencer_envelopestate.current = (ADC_getThresholdCV() >> 2);
			if(sequencer_envelopestate.current >= 512){
				sequencer_envelopestate.current -= 512;
				sequencer_envelopestate.current <<= 1;
			}else{
				sequencer_envelopestate.current = 0;
			}
			movingAverageFilter_process(&sequencer_thresholdCV_ma, (float)(sequencer_envelopestate.current - sequencer_envelopestate.previous));

			sequencer_envelopestate.counter++;
			if(sequencer_thresholdCV_ma.output > 0.0){
				if((sequencer_envelopestate.current >= sequencer_envelopethreshold)&&(sequencer_envelopethreshold > sequencer_envelopestate.previous)&&(sequencer_envelopestate.counter > 10)){
					sequencer_setNextStep();
					sequencer_envelopestate.counter = 0;
				}
			}
		}
	}
}

void sequencer_ui_init(void){
	for(uint32_t i=0; i < NUM_STEPS; i++){
		LED_setLow(steps[i].step_led);
	}
	sequencer_setStep(0);
}

void sequencer_setStep(uint16_t step){
	uint16_t previous_step = step - 1;
	if(previous_step >= NUM_STEPS){
		previous_step = NUM_STEPS - 1;
	}
	LED_setLow(steps[previous_step].step_led);
	LED_setHigh(steps[step].step_led);
	DAC_setValue(*steps[step].step_cv);
}

float sequencer_map_time_target(uint16_t CV, float min, float max) {
	return ((float)CV/SEQUENCER_CV_RESOLUTION * (max - min) + min);
}

void sequencer_processCV(uint32_t cv){
	movingAverageFilter_process(&sequencer_threshold_ma, (float) cv);

	sequencer_period = (uint16_t) (sequencer_map_time_target((uint16_t) sequencer_threshold_ma.output, SEQUENCER_MIN_PERIOD, SEQUENCER_MAX_PERIOD) / SEQUENCER_TIMER_PERIOD);
	sequencer_envelopethreshold = SEQUENCER_CV_RESOLUTION - (uint16_t) sequencer_threshold_ma.output;
}

void sequencer_updateDAC(uint16_t step){
	DAC_setValue(*steps[step].step_cv);
}

void sequencer_setMode(uint8_t routing){
	//see CVRouter, the CVRouter_getSource function returns 0 if nothing is assigned
	if(routing == 0){
		sequencer_mode = POT;
	}else{
		if(sequencer_envelopethreshold > SEQUENCER_CV_TRESHOLD){
			sequencer_mode = CV;
		}else{
			sequencer_mode = GATE;
		}
	}
}

void sequencer_setNextStep(void){
	sequencer_index++;
	if(sequencer_index >= NUM_STEPS){
		sequencer_index = 0;
	}
	sequencer_setStep(sequencer_index);
}
