/*
 * engine.c
 *
 */

#include "engine.h"

void engine_init(void) {
	/*
	 * user audio buffers setup
	 */
	audio_out_buffer_reset(&user_audio_out_buffer);
	audio_in_buffer_reset(&user_audio_in_buffer);
}
