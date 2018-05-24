/*
 * engine.h
 */

#ifndef ENGINE_H_
#define ENGINE_H_

/*
 * This file contains all the audio engine related variables and functions
 *
 */

#include "audioBuffers.h"

/*
 * General definitions
 */
#define F_SAMPLING 48000	//Attention, this definition is only used for the DSP calculations and not the codec setup, please configure the codes accrodingly


/*
 * Buffer/FIFO declarations
 */
audio_IO_buffer_t user_audio_out_buffer;
audio_IO_buffer_t user_audio_in_buffer;


/*
 * prototypes
 */
void engine_init(void);

#endif /* ENGINE_H_ */
