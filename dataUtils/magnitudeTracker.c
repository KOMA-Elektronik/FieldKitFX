/*
 * magnitudeTracker.c
 *
 */

#include "magnitudeTracker.h"

uint16_t trackTime = 0;
uint16_t trackMax = 0;

void magnitudeTracker_processSample(int16_t sample){
	uint16_t shiftedSample;
	//convert the sample from signed to unsigned
	if(sample & (1 << 15)){
		shiftedSample = ~(sample);
	}else{
		shiftedSample = sample;
	}
	//check wether we got a new maximum, otherwise increase the hold counter
	if(shiftedSample > trackMax){
		trackTime = 0;
		trackMax = shiftedSample;
	}else{
		trackTime++;
	}
	//check if the hold time elapsed
	if(trackTime > MAX_HOLD_TIME){
		trackTime = 0;
		trackMax = shiftedSample;
	}

}

uint16_t magnitudeTracker_getMax(void){
	return trackMax;
}
