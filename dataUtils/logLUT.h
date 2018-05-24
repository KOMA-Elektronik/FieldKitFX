/*
 * logLUT.h
 *
 */

#ifndef LOGLUT_H_
#define LOGLUT_H_

#include <stdint.h>

/*
 * a bunch of logarithmic lookup tables, used to map to differents pots and cvs
 */

const float invlogLUT_64[64];

const float logLUT_8bit[256];

const uint16_t inverselogLUT_10BitInt[1025];

const float inverselogLUT_10BitFloat[1025];

const uint16_t logLUT_10bitInt[1024];

const uint16_t logLUT_12bitInt[4096];

const uint16_t inverselogLUT_12bitInt[4096];

const float logLUT_12bit[4096];

#endif /* LOGLUT_H_ */
