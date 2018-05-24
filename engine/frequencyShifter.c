/*
 * frequencyShifter.c
 *
 */

#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "frequencyShifter.h"
#include "movingAverageFilter.h"
#include "ADC.h"
#include "freqCVLUT.h"
#include "randomUtils.h"


/*
 * Global definitions of the frequency shifter's components
 */
quadratureOscillator_t osc_;
HilbertTransformer_t hTrans_;
DCRemover_t DCRem_;
Xfader_t xfader_;
movingAverageFilterINT_t maf1;
movingAverageFilterINT_t maf2;
uint32_t shiftAmountMAFilterMem[FSMAFILTER_LEN];
uint32_t sidebandControlMAFilterMem[FSMAFILTER_LEN];

void frequencyShifter_init(frequencyShifter_t* freqShifter) {
	//populate the structure
	freqShifter->localOscillator = &osc_;
	freqShifter->transformer = &hTrans_;
	freqShifter->DCFilter = &DCRem_;
	freqShifter->xfader = &xfader_;
	freqShifter->shiftAmountMAFilter = &maf1;
	freqShifter->sidebandControlMAFilter = &maf2;

	//initialize the moving average filters
	movingAverageFilterINT_init(freqShifter->shiftAmountMAFilter, shiftAmountMAFilterMem, FSMAFILTER_LEN);
	movingAverageFilterINT_init(freqShifter->sidebandControlMAFilter, sidebandControlMAFilterMem, FSMAFILTER_LEN);

	//init the local oscillator
	quadratureOscillator_init(freqShifter->localOscillator);
	//init the hilbert transformer
	hilbertTransformer_init(freqShifter->transformer);
	//init the DC removing filter (AC coupler)
	DC_remover_init(freqShifter->DCFilter, 0.99f);

	//set the default shift amount to 0Hz
	frequencyShifter_setShiftAmt(freqShifter, 0);
}


void frequencyShifter_setShiftAmt(frequencyShifter_t* freqShifter, float freq) {
	quadratureOscillator_setFreq(freqShifter->localOscillator, freq);
}


void frequencyShifter_setSideBandXfade(frequencyShifter_t* freqShifter, float sidebandCoeff) {
	freqShifter->xfader->lambda = sidebandCoeff;
}


void frequencyShifter_setFbkAmt(frequencyShifter_t* freqShifter, float fbkAmt_) {
	freqShifter->fbkAmt = fbkAmt_;
}


void frequencyShifter_processSample(frequencyShifter_t* freqShifter, int16_t* sample) {
	float sample_I, sample_Q, sample_USB, sample_LSB;
	//convert the incoming sample to float
	sample_I = (float)(*sample)/32768.0;	//from [-32768, 32767] to [-1.0, 1.0]
	//remove DC component
	DC_remover_process(freqShifter->DCFilter, sample_I);
	//process the sample thru the hilbert transformer
	hilbertTransformer_process(freqShifter->transformer, sample_I);
	//update the oscillator
	quadratureOscillator_update(freqShifter->localOscillator);

	//compute the USB and LSB samples
	sample_I = freqShifter->localOscillator->sineOut * freqShifter->transformer->outputSample_1;
	sample_Q = freqShifter->localOscillator->cosineOut * freqShifter->transformer->outputSample_2;

	sample_USB = sample_I - sample_Q;
	sample_LSB = sample_I + sample_Q;

	//crossfade the two sideband signals and scale the output sample back to int16_t
	sample_I = freqShifter->xfader->lambda*sample_USB + (1.0-freqShifter->xfader->lambda)*sample_LSB;

	if(sample_I > 1.0)
		sample_I = 1.0;
	else if(sample_I < -1.0)
		sample_I = -1.0;

	*sample = sample_I*32767;
}

void frequencyShifter_updateCVs(frequencyShifter_t* freqShifter) {
	//scale the CV values and update the CV filters
	volatile uint32_t temp_amount = ADC_getMixedCV2() &(0xff0);

	/*
	 * WITH CV FILTERING
	 */

	movingAverageFilterINT_process(freqShifter->shiftAmountMAFilter, temp_amount);

	movingAverageFilterINT_process(freqShifter->sidebandControlMAFilter, ADC_getMixedCV1());

	//then make the actual changes
	frequencyShifter_setShiftAmt(freqShifter, freqCVLUT[ADC_RESOLUTION_DEZ - freqShifter->shiftAmountMAFilter->output]);
	frequencyShifter_setSideBandXfade(freqShifter, (float)(freqShifter->sidebandControlMAFilter->output)/(float)(ADC_RESOLUTION_DEZ));
}
