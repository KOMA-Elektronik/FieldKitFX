/*
 * looper.h
 *
 * The Looper is controlled trough a state machine, which reacts to user input
 * according to the current state.
 * The Effects, Sample Rate Reduction and Bitcrusher, are handled within separate functions.
 * Altering those effects might be the easiest way to start editing the code.
 */

#ifndef LOOPER_H_
#define LOOPER_H_

#include <stdint.h>
#include <math.h>
#include "memory.h"

#define LOOPER_FS 48000

//maximum frame number for USER_AUDIO_IO_BUFFER_SIZE=24 and 12 blocks à 3bytes (24bits) per frame
#define MAX_FRAME_NUM 7281

#define FRAME_NUMBER_FADE_IN 6

#define BLINK_FRAME_THRESHOLD 400
#define BLINK_GUARD_INTERVAL 400

/*
 * Effects
 */
#define LOOPER_MIN_BITDEPTH 1
#define LOOPER_MAX_DECIMATION_FACTOR 8

#define BITCRUSHER_LOWER_LIMIT -10
#define BITCRUSHER_UPPER_LIMIT 10

typedef enum {
	ARMED = 0,
	RECORD,
	PLAYBACK,
	OVERDUB,
	ERASE
} looperState_t;



typedef struct {
	RAMChip_t* memory;

	looperState_t state, previousState;
	uint16_t framePointer;
	uint16_t endFramePosition;
	uint8_t flag;
	uint8_t doFadeIn;
	uint8_t doFadeOut;
	uint8_t doOverdubFadeIn;
	uint8_t doOverdubFadeOut;
} looper_t;

looper_t looper;

void looper_init(looper_t* l);
void looper_process(looper_t * l);
void looper_process_bitcrush(int16_t* inputBuffer, int16_t* outputBuffer, uint16_t ADCValue);
void looper_process_sampleRateReducer(int16_t* inputBuffer, int16_t* outputBuffer, uint16_t ADCValue);

#endif /* LOOPER_H_ */
