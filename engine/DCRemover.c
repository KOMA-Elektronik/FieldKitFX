/*
 * DCRemover.c
 *
 */

#include "DCRemover.h"


void DC_remover_init(DCRemover_t* remover, float pole) {
	remover->pole = pole;
	remover->old_x = 0.0f;
	remover->old_y = 0.0f;
}


float DC_remover_process(DCRemover_t* remover, float inputSample) {
	float temp = (inputSample-remover->old_x)+(remover->old_y*remover->pole);
	remover->old_x = inputSample;
	remover->old_y = temp;
	return temp;
}
