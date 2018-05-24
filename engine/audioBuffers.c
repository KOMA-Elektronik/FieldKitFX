/*
 * audioBuffers.c
 *
 */

#include "audioBuffers.h"

/*
 * Output buffer functions
 */

uint8_t audio_out_buffer_isFull(audio_IO_buffer_t* buf) {
	return (buf->index >= AUDIO_IO_BUFFER_SIZE/4);
}

uint8_t audio_out_buffer_isEmpty(audio_IO_buffer_t* buf) {
	return (buf->index == 0);
}

void audio_out_buffer_push(audio_IO_buffer_t* buf, int16_t sample) {
	(buf->buffer)[buf->index] = sample;
	(buf->index)++;
}

void audio_out_buffer_reset(audio_IO_buffer_t* buf) {
	buf->index = 0;
}



/*
 * Input buffer functions
 */

uint8_t audio_in_buffer_isFull(audio_IO_buffer_t* buf) {
	return (buf->index == 0);
}

uint8_t audio_in_buffer_isEmpty(audio_IO_buffer_t* buf) {
	return (buf->index >= AUDIO_IO_BUFFER_SIZE/4);
}

void audio_in_buffer_pop(audio_IO_buffer_t* buf, int16_t* sample) {
	*sample = (buf->buffer)[buf->index];
	(buf->index)++;
}

void audio_in_buffer_reset(audio_IO_buffer_t* buf) {
	buf->index = 0;
}
