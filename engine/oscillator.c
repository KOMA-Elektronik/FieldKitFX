/*
 * oscillator.c
 *
 */


#include "oscillator.h"
#include "wavetables.h"
#include "movingAverageFilter.h"

void quadratureOscillator_init(quadratureOscillator_t* quadOsc) {
	quadOsc->phaseAcc = 0;			//to always start at the same phase (beginning of the wavetable)
	quadOsc->sinewave = sineWT;
	quadOsc->cosinewave = cosineWT;
}

void quadratureOscillator_setFreq(quadratureOscillator_t* quadOsc, float freq) {
	//set the frequency field
	quadOsc->frequency = freq;
	//check the sign of the new frequency
	//negative frequencies
	if(freq < 0.0f) {
		quadOsc->freq_neg = 1;

		quadOsc->increment = (uint32_t)((float)OSC_OVF_VALUE*(((-freq))/(float)F_SAMPLING));
	}

	//positive frequencies
	//basically the same deal than for the negative ones (just a sign thing on the sine output)
	else {
		quadOsc->freq_neg = 0;

		quadOsc->increment = (uint32_t)((float)OSC_OVF_VALUE*((freq)/(float)F_SAMPLING));
	}
}

void quadratureOscillator_update(quadratureOscillator_t* quadOsc) {
	//check if we should read the LUT backwards (neg freq) or not
	if(quadOsc->freq_neg == 1) {
		quadOsc->phaseAcc -= quadOsc->increment;
	}
	else {
		quadOsc->phaseAcc += quadOsc->increment;
	}

	float temp = quadOsc->phaseAcc * OSC_PHASE_FACTOR;
	uint32_t integerPart = temp;	//the integer part of the index
	float fractionnalPart = temp - integerPart;


	//sample interpolation (linear)
	//the interpolation has to follow the LUT reading direction (forward or backward)
	int16_t nextIndex;

	if(quadOsc->freq_neg == 1) {
		nextIndex = integerPart-1;
		if(nextIndex < 0) {	//if we arrive at the end of the wavetable
			nextIndex += OSC_RES;
		}
#ifdef OSC_INTERP
		float sample1 = (quadOsc->sinewave)[nextIndex];
		float sample2 = (quadOsc->sinewave)[integerPart];
		quadOsc->sineOut = (sample1 + (sample2 - sample1)*fractionnalPart);
#else
		quadOsc->sineOut  = (quadOsc->sinewave)[integerPart];
#endif
#ifdef OSC_INTERP
		sample1 = (quadOsc->cosinewave)[nextIndex];
		sample2 = (quadOsc->cosinewave)[integerPart];
		quadOsc->cosineOut = sample1 + (sample2 - sample1)*fractionnalPart;
#else
		quadOsc->cosineOut = (quadOsc->cosinewave)[integerPart];
#endif
	}
	else {
		nextIndex = integerPart+1;
		if(nextIndex >= OSC_RES) {	//if we arrive at the end of the wavetable
			nextIndex = 0;
		}
#ifdef OSC_INTERP
		float sample1 = (quadOsc->sinewave)[integerPart];
		float sample2 = (quadOsc->sinewave)[nextIndex];
		quadOsc->sineOut = (sample1 + (sample2 - sample1)*fractionnalPart);
#else
		quadOsc->sineOut  = (quadOsc->sinewave)[integerPart];
#endif
#ifdef OSC_INTERP
		sample1 = (quadOsc->cosinewave)[integerPart];
		sample2 = (quadOsc->cosinewave)[nextIndex];
		quadOsc->cosineOut = sample1 + (sample2 - sample1)*fractionnalPart;
#else
		quadOsc->cosineOut  = (quadOsc->cosinewave)[integerPart];
#endif
	}


}
