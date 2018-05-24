/*
 * frequencyShifter.h
 *
 * A frequency shifter based one ssb-modulation and iir-hilbert-transformer.
 */

#ifndef FREQUENCYSHIFTER_H_
#define FREQUENCYSHIFTER_H_

/*
 * DSP blocks includes
 */
#include "hilbertTransformer.h"
#include "Xfader.h"
#include "DCRemover.h"
#include "oscillator.h"
#include "movingAverageFilter.h"

/*
 * Constants definition
 */
#define MAX_FBKAMT 0.33f
#define FSMAFILTER_LEN 8
#define FREQLOCK_THRESHOLD 8

/*
 * Structure definition
 */
typedef struct {
	//	The CV input filters
	movingAverageFilterINT_t* shiftAmountMAFilter;
	movingAverageFilterINT_t* sidebandControlMAFilter;

	quadratureOscillator_t* localOscillator;
	HilbertTransformer_t* transformer;
	Xfader_t* xfader;
	DCRemover_t* DCFilter;
	float fbkAmt;
	float inputSample, outputSample, fbkSample;
} frequencyShifter_t;

/*
 * Structure's functions definitions
 */
void frequencyShifter_init(frequencyShifter_t* freqShifter);
void frequencyShifter_setShiftAmt(frequencyShifter_t* freqShifter, float freq);
void frequencyShifter_setSideBandXfade(frequencyShifter_t* freqShifter, float sidebandCoeff); //coeff € [0, 1] -> [LSB, DSB, USB]
void frequencyShifter_setFbkAmt(frequencyShifter_t* freqShifter, float fbkAmt_);
void frequencyShifter_processSample(frequencyShifter_t* freqShifter, int16_t* sample);
void frequencyShifter_updateCVs(frequencyShifter_t* freqShifter);

#endif /* FREQUENCYSHIFTER_H_ */
