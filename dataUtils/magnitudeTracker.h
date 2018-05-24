/*
 * magnitudeTracker.h
 *
 */

#ifndef MAGNITUDETRACKER_H_
#define MAGNITUDETRACKER_H_

#include <stdint.h>

/*
 * A fairly simple magnitude tracker. Used in the calibration mode to track
 * the magnitude of the incoming audio signal.
 */

//this defines the time (in samples) that a maximum value is hold
#define MAX_HOLD_TIME 24000

void magnitudeTracker_processSample(int16_t sample);
uint16_t magnitudeTracker_getMax(void);

#endif /* MAGNITUDETRACKER_H_ */
