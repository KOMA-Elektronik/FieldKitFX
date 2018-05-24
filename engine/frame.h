/*
 * frame.h
 *
 * Simple frame decoding system to safe some space in memory.
 * A block contains two 12bit samples in arranged into three bytes.
 * The frame size has to be adjusted according to the buffer size of the system making use of the encoding.
 */

#ifndef FRAME_H_
#define FRAME_H_

//based on USER_AUDIO_IO_BUFFER_SIZE=24 and 2 samples per block
#define BLOCKS_PER_FRAME 12


#define BLOCK_SIZE 3

#define FRAME_BYTECOUNT (BLOCK_SIZE * BLOCKS_PER_FRAME)

#include <stdint.h>

typedef struct{
	uint8_t nibbleByte;
	uint8_t lesser_samples[2];
} sample_block_t;

typedef struct {
	sample_block_t samples[BLOCKS_PER_FRAME];
} frame_t;


void encodeFrame(int16_t* rawStream, frame_t* frame);
void decodeFrame(frame_t* frame, int16_t* decodedStream);

#endif /* FRAME_H_ */
