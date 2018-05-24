/*
 * movingAverageFilter.c
 *
 */

#include "movingAverageFilter.h"
#include "stm32f3xx_hal.h"

void movingAverageFilter_init(movingAverageFilter_t *filter, float *array, uint32_t filterSize) {
	filter->length = filterSize;
	filter->line = array;

	//fill the array with zeros
	for(int i=0; i<filter->length; i++) {
		(filter->line)[i] = 0;
	}
	filter->oldy = 0;
}

void movingAverageFilter_process(movingAverageFilter_t *filter, float inputSample) {
	//shift the line
	for(int i=filter->length-1; i>0; i--)
		(filter->line)[i] = (filter->line)[i-1];
	//insert the new sample
	(filter->line)[0]=inputSample;
	//calculate the average value
	float temp = filter->oldy + (inputSample/filter->length) - ((filter->line)[filter->length-1]/filter->length);
	filter->oldy = temp;
	filter->output = temp;
}

void movingAverageFilterINT_init(movingAverageFilterINT_t *filter, uint32_t *array, uint32_t filterSize) {
  filter->length = filterSize;
  filter->line = array;

  //fill the array with zeros
  for(int i=0; i<filter->length; i++) {
    (filter->line)[i] = 0;
  }
  filter->oldy = 0;
}

void movingAverageFilterINT_process(movingAverageFilterINT_t *filter, uint32_t inputSample) {


  //shift the line
  for(int i=filter->length-1; i>0; i--)
    (filter->line)[i] = (filter->line)[i-1];
  //insert the new sample
  (filter->line)[0]=inputSample;
  //calculate the average value
  uint32_t temp = filter->oldy + (inputSample/filter->length) - ((filter->line)[filter->length-1]/filter->length);
  filter->oldy = temp;
  filter->output = temp;
}
