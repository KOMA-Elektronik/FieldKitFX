/*
 * randomUtils.c
 *
 */

#include "randomUtils.h"

float clip(float input) {
	if(input > 1.0)
		return 1.0;
	else if(input < -1.0)
		return -1.0;
	else
		return input;
}


float scale_uint12_to_float(uint16_t input){
	return (((float)input / 2047.0) - 1.0);
}

uint16_t scale_float_to_uint12(float input){
	return (uint16_t)((input + 1.0) * 2047);
}
