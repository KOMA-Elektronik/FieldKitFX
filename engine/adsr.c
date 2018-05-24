/*
 * adsr.c
 *
 */

#include "adsr.h"

void ADSR_init(ADSR_t* env) {
	env->accumulator = 0;
	env->state = IDLE;
	env->length = 1000;
	env->process_ui = 1;
	env->mode = POT_ADSR;
	movingAverageFilter_init(&envelope_treshold_ma,envelope_treshold_ma_array,ADSR_THRSH_MA_LENGTH);
	movingAverageFilter_init(&envelope_potA_ma,envelope_potA_ma_array, ADSR_POTA_MA_LENGTH);
	movingAverageFilter_init(&envelope_potD_ma,envelope_potD_ma_array, ADSR_POTD_MA_LENGTH);
	movingAverageFilter_init(&envelope_potR_ma,envelope_potR_ma_array, ADSR_POTR_MA_LENGTH);
}

void ADSR_ui_init(void){
	LED_setLow(&led0);
	LED_setLow(&led1);
	LED_setLow(&led2);
	LED_setLow(&led3);
}


void ADSR_processGate(ADSR_t* env, Gate_t* gate) {
	if(Gate_isHigh(gate)){
		env->gateIn = 1;
	}
	else {
		env->gateIn = 0;
	}
}


void ADSR_update(ADSR_t* env) {
	env->tick++;//increase tick counter
	switch(env->state) {
	case ATTACK:
		env->accumulator += env->A_inc;
		if(env->accumulator >= ADSR_ACC_MAX_VAL) {
			LED_setLow(&led0);
			LED_setHigh(&led1);
			env->state = DECAY;
			env->accumulator = ADSR_ACC_MAX_VAL;
		}
		if((env->mode == GATE_ADSR)||(env->mode == CV_ADSR)){
			if(env->gateIn == 0){
				LED_setLow(&led0);
				LED_setHigh(&led3);
				env->state = RELEASE;
				ADSR_setR_output(env);
			}
		}
		break;

	case DECAY:
		env->accumulator -= env->D_inc;
		if(env->accumulator <= env->S_lev) {
			env->state = SUSTAIN;
			LED_setLow(&led1);
			LED_setHigh(&led2);
			env->accumulator = env->S_lev;
		}
		//check wether decay might take the full envelope time and if so switch to release
		if(env->mode == POT_ADSR){
			if(env->tick >= (env->length * ADSR_FS)){
				env->state = RELEASE;
				LED_setLow(&led1);
				LED_setHigh(&led3);
				ADSR_setR_output(env);
			}
		}else if((env->mode == GATE_ADSR)||(env->mode == CV_ADSR)){
			if(env->gateIn == 0){
				LED_setLow(&led1);
				LED_setHigh(&led3);
				env->state = RELEASE;
				ADSR_setR_output(env);
			}
		}

		break;

	case SUSTAIN:
		if(env->mode == POT_ADSR){
			if(env->tick >= (env->length * ADSR_FS)){
				env->state = RELEASE;
				LED_setLow(&led2);
				LED_setHigh(&led3);
			}
		}else if((env->mode == GATE_ADSR)||(env->mode == CV_ADSR)){
			if((env->gateIn == 0)||(env->tick > 30000)){
				env->state = RELEASE;
				LED_setLow(&led2);
				LED_setHigh(&led3);
			}
		}

		break;

	case RELEASE:
		env->accumulator -= env->R_inc;
		if(env->accumulator <= 0.0) {
			env->state = IDLE;
			LED_setLow(&led3);
			env->accumulator = 0.0;
		}
		if((env->mode == GATE_ADSR)||(env->mode == CV_ADSR)){
			if(env->gateIn == 1){//retriggering
				env->state = ATTACK;
				LED_setHigh(&led0);
				LED_setLow(&led3);
				env->tick = 0;//reset tick counter
				env->process_ui = 1;
			}
		}
		break;

	case IDLE:
		if(env->mode == POT_ADSR){
			env->state = ATTACK;
			LED_setHigh(&led0);
			env->tick = 0;//reset tick counter
			env->process_ui = 1;
		}else if((env->mode == GATE_ADSR)||(env->mode == CV_ADSR)){
			if(env->gateIn == 1){
				env->state = ATTACK;
				LED_setHigh(&led0);
				env->tick = 0;//reset tick counter
				env->process_ui = 1;
			}else{
				env->process_ui = 1;
			}
		}else{
			env->process_ui = 1;
		}

		break;
	}

	//only take the 12 important bits of the accumulator
	env->output = env->accumulator * 4095;
}

float ADSR_map_time_target(uint16_t CV, float min, float max) {
	return ((float)CV/ADSR_CV_RESOLUTION * (max - min) + min);
}

void ADSR_processThresholdPot(ADSR_t* env, uint16_t CV1){
	if(env->mode == POT_ADSR){
		movingAverageFilter_process(&envelope_treshold_ma,inverselogLUT_10BitFloat[(uint16_t)ADSR_CV_RESOLUTION - CV1]);
	}else if(env->mode == CV_ADSR){
		movingAverageFilter_process(&envelope_treshold_ma,(uint16_t)ADSR_CV_RESOLUTION - CV1);
	}
}

void ADSR_setLengthThreshold(ADSR_t* env){
	if(env->mode == POT_ADSR){
		env->length = ADSR_map_time_target((uint16_t)envelope_treshold_ma.output, ADSR_MIN_OVERALL_TIME, ADSR_MAX_OVERALL_TIME);
		if(env->length < ADSR_MIN_OVERALL_TIME){
			env->length = ADSR_MIN_OVERALL_TIME;
		}
	}else if(env->mode == CV_ADSR){
		env->threshold_cv = envelope_treshold_ma.output;
		if(env->threshold_cv < 0){
			env->threshold_cv = 0;
		}
	}
}

void ADSR_processThresholdCV(ADSR_t* env, uint16_t CV){
	env->gatePrevious = env->gateIn;
	env->deadtime_counter++;
	if(CV >= 512){
		CV -= 512;
		CV <<= 1;
	}else{
		CV = 0;
	}
	if(CV > env->threshold_cv){
		if(env->gatePrevious == 0){//rising edge
			if(env->deadtime_counter > ADSR_DEADTIME){
				env->gateIn = 1;
				env->deadtime_counter = 0;
			}
		}
	}else{
		if(env->gatePrevious == 1){//falling edge
			if(env->deadtime_counter > ADSR_DEADTIME){
				env->gateIn = 0;
				env->deadtime_counter = 0;
			}
		}
	}
}

void ADSR_processADSRCV(ADSR_t* env, uint16_t CVA, uint16_t CVD, uint16_t CVS, uint16_t CVR){
	movingAverageFilter_process(&envelope_potA_ma, inverselogLUT_10BitFloat[CVA]);
	env->A_cv = envelope_potA_ma.output;
	if(env->A_cv < 0){
		env->A_cv = 0;
	}
	movingAverageFilter_process(&envelope_potD_ma, inverselogLUT_10BitFloat[CVD]);
	env->D_cv = envelope_potD_ma.output;
	if(env->D_cv < 0){
		env->D_cv = 0;
	}
	env->S_cv = (float)CVS;
	movingAverageFilter_process(&envelope_potR_ma, inverselogLUT_10BitFloat[CVR]);
	env->R_cv = envelope_potR_ma.output;
	if(env->R_cv < 0){
		env->R_cv = 0;
	}
}

void ADSR_setA(ADSR_t* env) {
	volatile float target = ADSR_map_time_target((uint16_t)env->A_cv, ADSR_MIN_A_TIME, ADSR_MAX_A_TIME);
	env->A_inc = (ADSR_ACC_MAX_VAL/(target*ADSR_FS));
}

void ADSR_setD(ADSR_t* env) {
	volatile float target = ADSR_map_time_target((uint16_t)env->D_cv, ADSR_MIN_D_TIME, ADSR_MAX_D_TIME);
	env->D_inc = (ADSR_ACC_MAX_VAL-(env->S_lev))/(target*ADSR_FS);
}

void ADSR_setS(ADSR_t* env){
	env->S_lev = (float)env->S_cv/ADSR_CV_RESOLUTION;
}

void ADSR_setR(ADSR_t* env) {
	volatile float target = ADSR_map_time_target((uint16_t)env->R_cv, ADSR_MIN_R_TIME, ADSR_MAX_R_TIME);
	env->R_inc = env->S_lev/(target*ADSR_FS);
}

//this recalculates the R_inc based on the current output, that is needed when the decay stage overlaps the overall length of the envelope
void ADSR_setR_output(ADSR_t* env){
	volatile float target = ADSR_map_time_target(env->R_cv, ADSR_MIN_R_TIME, ADSR_MAX_R_TIME);
	env->R_inc = env->accumulator/(target*ADSR_FS);
}

void ADSR_setMode(ADSR_t* env, uint32_t cv, uint8_t routing){
	//see CVRouter, the state function returns 0 if nothing is assigned
	if(routing == 0){
		env->mode = POT_ADSR;
	}else{
		if(ADSR_CV_TRESHOLD_SCALED < ADSR_CV_RESOLUTION - cv){
			env->mode = CV_ADSR;
		}else{
			env->mode = GATE_ADSR;
		}
	}
}

void ADSR_processSample(ADSR_t* env){
	//Check gate input
	if(env->mode == GATE_ADSR){
		ADSR_processGate(&rollo_env, &gate);
	}else if(env->mode == CV_ADSR){
		ADSR_processThresholdCV(&rollo_env, ADC_getThresholdCV() >> 2);
	}
	//		update the ADSR output
	ADSR_update(&rollo_env);
	//		put that to the DAC
	DAC_setValue(rollo_env.output);
}
