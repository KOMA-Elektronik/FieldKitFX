/*
 * audioBuffers.h
 *
 * Buffers used for the audio i/o-stream.
 * The division by 4 results from using only on channel of the stereo stream (/2) and
 * the DMA writing interval of half-buffers (/2)
 */

#ifndef AUDIOBUFFERS_H_
#define AUDIOBUFFERS_H_

#include <stdint.h>

#define AUDIO_IO_BUFFER_SIZE 96	//divide by 4 to have the audio latency (in samples)
#define USER_AUDIO_IO_BUFFER_SIZE AUDIO_IO_BUFFER_SIZE/4


typedef struct {
	int16_t buffer[USER_AUDIO_IO_BUFFER_SIZE];
	uint8_t index;
} audio_IO_buffer_t;

uint8_t audio_out_buffer_isFull(audio_IO_buffer_t* buf);
uint8_t audio_out_buffer_isEmpty(audio_IO_buffer_t* buf);
void audio_out_buffer_push(audio_IO_buffer_t* buf, int16_t sample);
void audio_out_buffer_reset(audio_IO_buffer_t* buf);

uint8_t audio_in_buffer_isFull(audio_IO_buffer_t* buf);
uint8_t audio_in_buffer_isEmpty(audio_IO_buffer_t* buf);
void audio_in_buffer_pop(audio_IO_buffer_t* buf, int16_t* sample);
void audio_in_buffer_reset(audio_IO_buffer_t* buf);




#endif /* AUDIOBUFFERS_H_ */
