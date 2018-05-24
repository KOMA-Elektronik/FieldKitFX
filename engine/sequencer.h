/*
 * sequencer.h
 *
 */

#ifndef SEQUENCER_H_
#define SEQUENCER_H_

#include "LED.h"
#include "rollodecks.h"
#include "DAC.h"
#include "ADC.h"
#include "movingAverageFilter.h"
#include "../hardware/routingMatrix.h"
#include "Gate.h"
#include "randomUtils.h"

/*
 * DEFINES
 */

/*
 * The number of steps available to the sequencer
 */

#define NUM_STEPS 4

#define SEQUENCER_CV_RESOLUTION 1023

#define SEQUENCER_MIN_PERIOD 0.05
#define SEQUENCER_MAX_PERIOD 5.0

#define SEQUENCER_TIMER_PERIOD 0.0001

#define SEQUENCER_RISING_TRESHOLD 2

//this is the number of the actual button
#define SEQUENCER_CV_DESTINATION 10

#define SEQUENCER_CV_TRESHOLD 50

//moving average filer for the threshold pot
#define SEQUENCER_THRSH_MA_LENGTH 30
movingAverageFilter_t sequencer_threshold_ma;
float sequencer_threshold_ma_array[SEQUENCER_THRSH_MA_LENGTH];

//moving average filer for the threshold cv
#define SEQUENCER_THRSHCV_MA_LENGTH 10
movingAverageFilter_t sequencer_thresholdCV_ma;
float sequencer_thresholdCV_ma_array[SEQUENCER_THRSHCV_MA_LENGTH];

typedef enum{
	POT,
	CV,
	GATE
} seqmode_t;


/*
 * Declaration of the step-struct, allows to connect one LED to one CV-source
 */
typedef struct {
	LED_t* step_led;
	uint16_t* step_cv;
} step_t;

/*
 * Declaration of an array that holds all the steps
 */

step_t steps[NUM_STEPS];

/*
 * Declaration of the sequencer_period, which defines the speed of the sequencer
 * this value is compared against the counter register of the timer
 */
uint16_t sequencer_period;
uint16_t sequencer_timepassed;
uint16_t sequencer_index;
seqmode_t sequencer_mode;

typedef struct {
	uint16_t previous;
	uint16_t current;
	uint16_t counter;
} envelope_state_t;

envelope_state_t sequencer_envelopestate;

uint16_t sequencer_envelopethreshold;

void sequencer_Init(void);

void sequencer_ui_init(void);

void sequencer_setStep(uint16_t step);

void sequencer_watch(void);

void sequencer_processCV(uint32_t cv);

void sequencer_updateDAC(uint16_t step);

void sequencer_setMode(uint8_t routing);

float sequencer_map_time_target(uint16_t CV, float min, float max);

void sequencer_setNextStep(void);

#endif /* SEQUENCER_H_ */
