/*
 * hilbertTransformer.h
 *
 */

#ifndef HILBERTTRANSFORMER_H_
#define HILBERTTRANSFORMER_H_

/*
 *
 *	Series allpass filters based Hilbert transformer
 *
 *		|------BR1----- out_1
 *	----|
 *		|------BR2----- out_2
 *
 *	each branch is composed of 6 1st order allpass filters in series
 *	the coefficients were pre-computed in matlab beforehand. See https://github.com/csound/csound/blob/develop/Opcodes/ugsc.c#L120
 *
 *	The first 6 coefficients of the "coefficients" field of the structure are the ones related to the first branch, the rest is for the 2nd branch
 */

#define NUM_ALLPASS 12
#define NUM_ALLPASS_BRANCH 6

typedef struct {
	float oldx[NUM_ALLPASS];
	float oldy[NUM_ALLPASS];
	const float* coefficients;
	//the output samples of the filter
	float outputSample_1, outputSample_2;
} HilbertTransformer_t;

void hilbertTransformer_init(HilbertTransformer_t* ht);
void hilbertTransformer_process(HilbertTransformer_t* ht, float inputSample);

#endif /* HILBERTTRANSFORMER_H_ */
