/*
 * looper.c
 *
 */

#include "looper.h"
#include "engine.h"
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "ADC.h"
#include "logLUT.h"
#include "fadeLUT.h"

void looper_init(looper_t* l) {
	l->state = ARMED;
	l->doFadeIn = 0;
	l->doFadeOut = 0;
	l->doOverdubFadeIn = 0;
	l->doOverdubFadeOut = 0;
}


void looper_process(looper_t * l) {
	uint8_t i;
	frame_t processingFrame;
	int16_t looperSampleBuffer_dry[USER_AUDIO_IO_BUFFER_SIZE];
	float sampleFloat;

	switch(l->state) {
	case ARMED:
		//empty the buffer
		user_audio_in_buffer.index = USER_AUDIO_IO_BUFFER_SIZE;
		//compress to frame
		encodeFrame(user_audio_in_buffer.buffer, &processingFrame);
		//decompress frame
		decodeFrame(&processingFrame, user_audio_out_buffer.buffer);
		user_audio_out_buffer.index = AUDIO_IO_BUFFER_SIZE/4;
		break;
	case RECORD:
		//empty the buffer
		user_audio_in_buffer.index = USER_AUDIO_IO_BUFFER_SIZE;
		//fade the first six frames (3ms) in
		if(l->framePointer < FRAME_NUMBER_FADE_IN){
			for(uint8_t k=0; k<USER_AUDIO_IO_BUFFER_SIZE; k++) {
				sampleFloat = (float)user_audio_in_buffer.buffer[k];
				sampleFloat *= fadeLUT144[k + l->framePointer * 24];
				user_audio_in_buffer.buffer[k] = (int16_t)sampleFloat;
			}
		}

		//compress to frame
		encodeFrame(user_audio_in_buffer.buffer, &processingFrame);
		//write frame to RAM
		RAM_writeFrame(l->memory, l->framePointer, &processingFrame);
		//update frame pointer
		l->framePointer ++;
		//decompress the frame and toss it in the output buffer
		decodeFrame(&processingFrame, user_audio_out_buffer.buffer);
		user_audio_out_buffer.index = AUDIO_IO_BUFFER_SIZE/4;
		break;
	case PLAYBACK:
		//discard input buffer
		user_audio_in_buffer.index = USER_AUDIO_IO_BUFFER_SIZE;
		//compress to frame
		encodeFrame(user_audio_in_buffer.buffer, &processingFrame);
		//decompress frame
		decodeFrame(&processingFrame, user_audio_out_buffer.buffer);
		//get frame from RAM
		RAM_readFrame(l->memory, l->framePointer, &processingFrame);
		//		update frame pointer
		l->framePointer++;
		if(l->framePointer > l->endFramePosition) {
			l->framePointer = 0;
		}
		//		decode the frame
		decodeFrame(&processingFrame, looperSampleBuffer_dry);
		//		decodeFrame(&processingFrame, user_audio_out_buffer.buffer);
		//compress to frame
		encodeFrame(user_audio_in_buffer.buffer, &processingFrame);
		//decompress frame
		decodeFrame(&processingFrame, user_audio_out_buffer.buffer);
		//add the contents of the buffer to the decoded frame
		for(i=0; i<USER_AUDIO_IO_BUFFER_SIZE; i++) {
			if ((looperSampleBuffer_dry[i] > 0 && user_audio_in_buffer.buffer[i] > INT16_MAX - looperSampleBuffer_dry[i]) ||
					(user_audio_in_buffer.buffer[i] > 0 && looperSampleBuffer_dry[i] > INT16_MAX - user_audio_in_buffer.buffer[i]))
			{
				/* Oh no, overflow */
				user_audio_out_buffer.buffer[i] = INT16_MAX;
			}
			else if((looperSampleBuffer_dry[i] < 0 && user_audio_in_buffer.buffer[i] < INT16_MIN - looperSampleBuffer_dry[i]) ||
					(user_audio_in_buffer.buffer[i] < 0 && looperSampleBuffer_dry[i] < INT16_MIN - user_audio_in_buffer.buffer[i]))
			{
				/* Oh no, underflow */
				user_audio_out_buffer.buffer[i] = INT16_MIN;
			}
			else
			{
				user_audio_out_buffer.buffer[i] = user_audio_in_buffer.buffer[i] + looperSampleBuffer_dry[i];
			}
		}

		if(l->doFadeOut){
			RAM_readFrame(l->memory, l->endFramePosition - l->doFadeOut + 1, &processingFrame);
			decodeFrame(&processingFrame, looperSampleBuffer_dry);
			//fade it out
			for(uint8_t k=0; k<USER_AUDIO_IO_BUFFER_SIZE; k++) {
				sampleFloat = (float)looperSampleBuffer_dry[k];
				sampleFloat *= fadeLUT144[(l->doFadeOut * 24 - 1) - k];
				looperSampleBuffer_dry[k] = (int16_t)sampleFloat;
			}
			//rewrite it to RAM
			encodeFrame(looperSampleBuffer_dry, &processingFrame);
			RAM_writeFrame(l->memory, l->endFramePosition - l->doFadeOut + 1, &processingFrame);
			l->doFadeOut--;
		}
		user_audio_out_buffer.index = AUDIO_IO_BUFFER_SIZE/4;
		break;

	case OVERDUB:
		//empty the buffer
		user_audio_in_buffer.index = USER_AUDIO_IO_BUFFER_SIZE;
		//get the next frame
		RAM_readFrame(l->memory, l->framePointer, &processingFrame);
		//decompress it
		decodeFrame(&processingFrame, looperSampleBuffer_dry);
		//if first overdubbed frames, fade them in
		if(l->doOverdubFadeIn) {
			for(uint8_t k=0; k<USER_AUDIO_IO_BUFFER_SIZE; k++) {
				sampleFloat = (float)user_audio_in_buffer.buffer[k];
				sampleFloat *= fadeLUT144[k + ((FRAME_NUMBER_FADE_IN - l->doOverdubFadeIn) * 24)];
				user_audio_in_buffer.buffer[k] = (int16_t)sampleFloat;
			}
			l->doOverdubFadeIn--;
		}
		if(l->doOverdubFadeOut){
			for(uint8_t k=0; k<USER_AUDIO_IO_BUFFER_SIZE; k++) {
				sampleFloat = (float)user_audio_in_buffer.buffer[k];
				sampleFloat *= fadeLUT144[(l->doOverdubFadeOut * 24 - 1) - k];
				user_audio_in_buffer.buffer[k] = (int16_t)sampleFloat;
			}
			l->doOverdubFadeOut--;
			if(l->doOverdubFadeOut == 0){
				l->state = PLAYBACK;
			}
		}
		//add the contents of the buffer to the decoded frame
		for(i=0; i<USER_AUDIO_IO_BUFFER_SIZE; i++) {
			if ((looperSampleBuffer_dry[i] > 0 && user_audio_in_buffer.buffer[i] > INT16_MAX - looperSampleBuffer_dry[i]) ||
					(user_audio_in_buffer.buffer[i] > 0 && looperSampleBuffer_dry[i] > INT16_MAX - user_audio_in_buffer.buffer[i]))
			{
				/* Oh no, overflow */
				looperSampleBuffer_dry[i] = INT16_MAX;
			}
			else if((looperSampleBuffer_dry[i] < 0 && user_audio_in_buffer.buffer[i] < INT16_MIN - looperSampleBuffer_dry[i]) ||
					(user_audio_in_buffer.buffer[i] < 0 && looperSampleBuffer_dry[i] < INT16_MIN - user_audio_in_buffer.buffer[i]))
			{
				/* Oh no, underflow */
				looperSampleBuffer_dry[i] = INT16_MIN;
			}
			else
			{
				looperSampleBuffer_dry[i] = user_audio_in_buffer.buffer[i] + looperSampleBuffer_dry[i];
			}
		}

		//compress back in frame
		encodeFrame(looperSampleBuffer_dry, &processingFrame);
		//write the frame
		RAM_writeFrame(l->memory, l->framePointer, &processingFrame);
		//update frame pointer
		l->framePointer++;
		if(l->framePointer > l->endFramePosition) {
			l->framePointer = 0;
		}
		//need to decode the frame to avoid buffer corruption
		decodeFrame(&processingFrame, looperSampleBuffer_dry);
		//send to output buffer
		for(i=0; i<USER_AUDIO_IO_BUFFER_SIZE; i++) {
			user_audio_out_buffer.buffer[i] = looperSampleBuffer_dry[i];
		}
		user_audio_out_buffer.index = AUDIO_IO_BUFFER_SIZE/4;
		break;
	case ERASE:
		//empty the input buffer
		user_audio_in_buffer.index = USER_AUDIO_IO_BUFFER_SIZE;
		//get the next frame
		RAM_readFrame(l->memory, l->framePointer, &processingFrame);
		//update frame pointer
		l->framePointer++;
		//decompress it to output buffer
		decodeFrame(&processingFrame, user_audio_out_buffer.buffer);
		user_audio_out_buffer.index = AUDIO_IO_BUFFER_SIZE/4;
		break;
	default:
		break;
	}
}

void looper_process_bitcrush(int16_t* inputBuffer, int16_t* outputBuffer, uint16_t ADCValue) {
	uint8_t i, bitReduction_integerPart;
	volatile float bitReduction = (logLUT_12bit[ADCValue]*(16.0-LOOPER_MIN_BITDEPTH))/4095.0;
	uint16_t tempSample1, tempSample2;

	bitReduction_integerPart = (uint8_t) bitReduction;

	for(i=0; i<USER_AUDIO_IO_BUFFER_SIZE; i++) {
		if((BITCRUSHER_LOWER_LIMIT > inputBuffer[i]) || (inputBuffer[i] > BITCRUSHER_UPPER_LIMIT)){
			tempSample1 = inputBuffer[i] + 32768;
			//shift
			tempSample1 >>= bitReduction_integerPart;
			tempSample1 <<= bitReduction_integerPart;

			tempSample2 = inputBuffer[i] + 32768;
			//shift
			tempSample2 >>= bitReduction_integerPart+1;
			tempSample2 <<= bitReduction_integerPart+1;

			tempSample1 += (tempSample2 - tempSample1)*(bitReduction - bitReduction_integerPart);

			//de-offset
			outputBuffer[i] = tempSample1 - 32768;
		}else{
			outputBuffer[i] = inputBuffer[i];
		}
	}
}

void looper_process_sampleRateReducer(int16_t* inputBuffer, int16_t* outputBuffer, uint16_t ADCValue) {
	const uint8_t decimationFactors[7] = {1, 2, 4, 8, 16, 32, 64};
	volatile float decimationFactor = ((ADCValue*6)/4095.0);

	uint8_t i, decimationFactor_integerPart;
	int16_t tempSample1, tempSample2;

	decimationFactor_integerPart = (uint8_t) decimationFactor;

	for(i=0; i<USER_AUDIO_IO_BUFFER_SIZE; i++) {
		if((i%decimationFactors[decimationFactor_integerPart]) == 0) {
			tempSample1 = inputBuffer[i];
		}
		if((i%(decimationFactors[decimationFactor_integerPart+1])) == 0) {
			tempSample2 = inputBuffer[i];
		}

		outputBuffer[i] = (int16_t) (tempSample1 + (tempSample2 - tempSample1)*(decimationFactor - decimationFactor_integerPart));
	}
}

void looper_linearInterp(int16_t* clean, int16_t* dirty, int16_t* out, uint16_t ADCValue) {
	uint8_t i;
	float  temp;
	for(i=0; i<USER_AUDIO_IO_BUFFER_SIZE; i++) {
		temp = clean[i]*(4095-ADCValue) + dirty[i]*ADCValue;
		out[i] = (int16_t)(temp/4095.0);
	}
}
