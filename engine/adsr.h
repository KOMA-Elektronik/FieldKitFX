/*
 * adsr.h
 *
 * The ADSR of the Field Kit FX.
 */

#ifndef ADSR_H_
#define ADSR_H_

#include <stdint.h>
#include "DAC.h"
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "LED.h"
#include "rolloSelector_switch.h"
#include "debug.h"
#include "routingMatrix.h"
#include "Gate.h"
#include "movingAverageFilter.h"
#include "logLUT.h"
#include "ADC.h"

//Definition of the extrema of all envelope stages. Times are given in seconds.
#define ADSR_MIN_A_TIME 0.001
#define ADSR_MAX_A_TIME 1.0
#define ADSR_MIN_D_TIME 0.01
#define ADSR_MAX_D_TIME 1.0
#define ADSR_MIN_R_TIME 0.01
#define ADSR_MAX_R_TIME 1.0
#define ADSR_MIN_OVERALL_TIME 0.01
#define ADSR_MAX_OVERALL_TIME 3.0


#define ADSR_ACC_MAX_VAL 1.0
#define ADSR_FS 10000	//to link w/ timer

#define ADSR_CV_RESOLUTION 1023.0

//Threshold to distinguish between "Gate-Mode" or "Envelope-Mode" when external CV is routed to Threshold Input
#define ADSR_CV_TRESHOLD_SCALED 5

//Deadtime in ADSR-Samples. Prevents retriggering within untrustworthy time spans (for example due to noise...)
#define ADSR_DEADTIME 50


//moving average filer for the threshold pot
#define ADSR_THRSH_MA_LENGTH 10
movingAverageFilter_t envelope_treshold_ma;
float envelope_treshold_ma_array[ADSR_THRSH_MA_LENGTH];

//moving average filter for the other pots
#define ADSR_POTA_MA_LENGTH 10
movingAverageFilter_t envelope_potA_ma;
float envelope_potA_ma_array[ADSR_POTA_MA_LENGTH];

#define ADSR_POTD_MA_LENGTH 10
movingAverageFilter_t envelope_potD_ma;
float envelope_potD_ma_array[ADSR_POTD_MA_LENGTH];

#define ADSR_POTR_MA_LENGTH 10
movingAverageFilter_t envelope_potR_ma;
float envelope_potR_ma_array[ADSR_POTR_MA_LENGTH];

typedef enum {
	IDLE = 0,
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE
} ADSRState_t;

typedef enum{
	POT_ADSR,
	CV_ADSR,
	GATE_ADSR
} ADSRmode_t;


typedef struct {
	ADSRState_t state;
	uint8_t gateIn;//flag to signalize wether the gate input is high or low
	uint8_t gatePrevious;//the previous state of the flag, needed to detect edges and base dead timing on those
	float accumulator;
	float A_inc, D_inc, S_lev, R_inc;
	float A_cv, D_cv, S_cv, R_cv;
	uint16_t tick; //time passed inside the envelope
	float length; //length of the envelope in s
	uint8_t process_ui; //flag to signalize wether ui-inputs should be processed
	uint16_t r_cv;//varible that holds the cv for release stage, needed for recalculation
	ADSRmode_t mode;//determines the mode of the adsr
	uint16_t output;	//12 bits
	float threshold_cv; //to be compared with an incoming signal
	uint16_t deadtime_counter; //prevents triggering the envelope in cv mode within an untrustworthy time span (for example due to noise...)
} ADSR_t;

ADSR_t rollo_env;

void ADSR_processGate(ADSR_t* env, Gate_t* gate);
void ADSR_init(ADSR_t* env);
void ADSR_ui_init(void);
void ADSR_update(ADSR_t* env);
float ADSR_map_time_target(uint16_t CV, float min, float max);
void ADSR_processThresholdPot(ADSR_t* env, uint16_t CV1);
void ADSR_processThresholdCV(ADSR_t* env, uint16_t CV);
void ADSR_setLengthThreshold(ADSR_t* env);
void ADSR_processADSRCV(ADSR_t* env, uint16_t CVA, uint16_t CVD, uint16_t CVS, uint16_t CVR);
void ADSR_setA(ADSR_t* env);
void ADSR_setD(ADSR_t* env);
void ADSR_setS(ADSR_t* env);
void ADSR_setR(ADSR_t* env);
void ADSR_setR_output(ADSR_t* env);
void ADSR_setMode(ADSR_t* env, uint32_t cv, uint8_t routing);

void ADSR_processSample(ADSR_t* env);

#endif /* ADSR_H_ */
