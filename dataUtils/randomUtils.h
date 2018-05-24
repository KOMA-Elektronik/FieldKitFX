/*
 * randomUtils.h
 *
 */

#ifndef RANDOMUTILS_H_
#define RANDOMUTILS_H_

#include <stdint.h>

/*
 * Random little utility functions to manipulate data
 */

/*
 * clips a float to [-1.0,1.0]
 */
float clip(float input);

/*
 * scales an unsigned 12bit signal to a float [-1.0,1.0]
 */
float scale_uint12_to_float(uint16_t input);

/*
 * scales a float [-1.0,1.0] to a unsigned 12bit signal
 */

uint16_t scale_float_to_uint12(float input);

#endif /* RANDOMUTILS_H_ */
