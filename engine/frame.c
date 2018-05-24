/*
 * frame.c
 *
 */

#include "frame.h"


void encodeFrame(int16_t* rawStream, frame_t* frame) {
	uint8_t block_counter;
	uint16_t temp1, temp2;

	for(block_counter=0; block_counter<BLOCKS_PER_FRAME; block_counter++) {
		//reduce to unsigned 12bit
		temp1 = ((rawStream[block_counter*2]/16) + 2048);
		temp2 = ((rawStream[block_counter*2+1]/16) + 2048);

		frame->samples[block_counter].nibbleByte = ((temp1 >> 4) & 0xf0) | (temp2 >> 8);
		frame->samples[block_counter].lesser_samples[0] = temp1 & 0x00ff;
		frame->samples[block_counter].lesser_samples[1] = temp2 & 0x00ff;
	}
}

void decodeFrame(frame_t* frame, int16_t* decodedStream) {
	int8_t block_counter;
	int16_t temp1, temp2;

	for(block_counter=0; block_counter<BLOCKS_PER_FRAME; block_counter++) {
		temp1 = ((frame->samples[block_counter].nibbleByte & 0xf0) << 4) | frame->samples[block_counter].lesser_samples[0];
		temp2 = ((frame->samples[block_counter].nibbleByte & 0x0f) << 8) | frame->samples[block_counter].lesser_samples[1];
		//convert to 16 bits
		decodedStream[block_counter*2] = ((temp1-2048)*16);
		decodedStream[block_counter*2+1] = ((temp2-2048)*16);
	}
}
