/*
 * movingAverageFilter.h
 *
 * Moving average filters designed to process either float or integers.
 * Mainly used for smoothing readings of Potentionmeters and external CVs.
 */

#ifndef MOVINGAVERAGEFILTER_H_
#define MOVINGAVERAGEFILTER_H_

#include "stdint.h"

typedef struct {
	float *line;
	uint32_t length;
	float oldy;
	float output;
}movingAverageFilter_t;

typedef struct {
  uint32_t *line;
  uint32_t length;
  uint32_t oldy;
  uint32_t output;
}movingAverageFilterINT_t;



void movingAverageFilter_init(movingAverageFilter_t *filter, float *array, uint32_t filterSize);
void movingAverageFilter_process(movingAverageFilter_t *filter, float inputSample);

void movingAverageFilterINT_init(movingAverageFilterINT_t *filter, uint32_t *array, uint32_t filterSize);
void movingAverageFilterINT_process(movingAverageFilterINT_t *filter, uint32_t inputSample);


#endif /* MOVINGAVERAGEFILTER_H_ */
