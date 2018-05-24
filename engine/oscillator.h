/*
 * oscillator.h
 *
 */

#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include "engine.h"

#define OSC_RES 1024	//size of the LUT

#define OSC_OVF_VALUE 4294967296	//2^32
#define OSC_PHASE_FACTOR ((float) OSC_RES)/( (float) OSC_OVF_VALUE)

//#define OSC_INTERP

typedef struct quadrature_oscillator_t {
	uint32_t phaseAcc;
	uint32_t increment;
	const float* sinewave;
	const float* cosinewave;
	uint8_t freq_neg;	//sign of the frequency, 0 if positive, 1 if negative
	//the outputs of the oscillator
	float sineOut, cosineOut;
	//for reference during the warping process
	float frequency;
} quadratureOscillator_t;


void quadratureOscillator_init(quadratureOscillator_t* quadOsc);
void quadratureOscillator_setFreq(quadratureOscillator_t* quadOsc, float freq);
void quadratureOscillator_update(quadratureOscillator_t* quadOsc);

#endif /* OSCILLATOR_H_ */
