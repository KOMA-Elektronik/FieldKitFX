/*
 * Xfader.c
 *
 */

#include "Xfader.h"

float Xfader_process(Xfader_t* xfader, float in1, float in2) {
	return xfader->lambda*in1 + (1.0-(xfader->lambda))*in2;
}
