/*
 * hilbertTransformer.c
 *
 */

#include "stdint.h"

/*
 * 		The structure of the hilbert transformer :
 *
 *
 *
 *
 * 						+---[AP]---[AP]---[AP]--	(+45° output)
 * 						|
 * 		IN -------------|
 * 						|
 * 						+---[AP]---[AP]---[AP]--	(-45° output)
 *
 *
 *
 */


#include "hilbertTransformer.h"


const float coefficients[NUM_ALLPASS] = {-0.999229004278298, -0.994158787551880, -0.976436246932096, -0.908711820239948, -0.677935627586549, -0.0792101408955407, -0.997327025721254, -0.988172698429361, -0.953364806814201, -0.825195989410729, -0.439051054845000, 0.494956506139769};


void hilbertTransformer_init(HilbertTransformer_t* ht) {
	uint8_t i;

	ht->coefficients = coefficients;

	for(i=0; i<NUM_ALLPASS; i++) {
		(ht->oldx)[i] = 0.0;
		(ht->oldy)[i] = 0.0;
	}
}

void hilbertTransformer_process(HilbertTransformer_t* ht, float inputSample) {
	uint8_t i;

	float y;
	float x = inputSample;

	//first branch
	for(i=0; i<NUM_ALLPASS_BRANCH; i++) {
		y = (ht->coefficients)[i] * (x - (ht->oldy)[i]) + (ht->oldx)[i];
		(ht->oldx)[i] = x;
		(ht->oldy)[i] = y;
		x = y;
	}
	ht->outputSample_1 = y;

	//second branch
	x = inputSample;
	for(i=NUM_ALLPASS_BRANCH; i<NUM_ALLPASS; i++) {
		y = (ht->coefficients)[i] * (x - (ht->oldy)[i]) + (ht->oldx)[i];
		(ht->oldx)[i] = x;
		(ht->oldy)[i] = y;
		x = y;
	}
	ht->outputSample_2 = y;
}
