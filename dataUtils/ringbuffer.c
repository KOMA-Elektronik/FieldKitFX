/*
 * ringbuffer.c
 *
 */

#include "ringbuffer.h"


/*
	Reset the ringbuffer
	to reset a ringbuffer, the tail must be at the same position tan it's head
 */
void ringbuf_reset(ringbufHandler_t *buffer) {
	buffer->tail = 0;
	buffer->head = buffer->tail;
	buffer->mask = (buffer->size)-1;
}



// Test if the buffer is full, returns true if yes, false otherwise
uint16_t ringbuf_isFull(ringbufHandler_t *buffer) {
	return (ringbuf_getUsedElements(buffer) > buffer->mask);	//buffer full
}


// Test if the buffer is empty, returns true if yes, false otherwise
uint16_t ringbuf_isEmpty(ringbufHandler_t *buffer) {
	return (buffer->head == buffer->tail);	//buffer empty
}

// returns the number of used elements in the buffer
uint16_t ringbuf_getUsedElements(ringbufHandler_t *buffer) {
	return (buffer->head - buffer->tail);
}


//	returns the number of available elements in the ring buffer
uint16_t ringbuf_getAvailableElements(ringbufHandler_t *buffer) {
	return (buffer->size - (ringbuf_getUsedElements(buffer)));
}


//	push data in the ringbuffer, where the head is at
//	returns 0 if buffer was full, otherwise returns the number of processed elements
uint16_t ringbuf_push(ringbufHandler_t *buffer, int16_t data) {
	uint16_t ret;
	ret = !ringbuf_isFull(buffer);
	if(ret) {
		buffer->bufferPointer[buffer->head & buffer->mask] = data;
		buffer->head++;
	}
	return ret;
}


//	pop data out of the buffer
//	returns 0 if buffer was empty, otherwise returns the number of processed elements
uint16_t ringbuf_pop(ringbufHandler_t *buffer, int16_t *data) {
	uint16_t ret;
	ret = !ringbuf_isEmpty(buffer);
	if(ret) {
		*data = buffer->bufferPointer[buffer->tail & buffer->mask];
		buffer->tail++;
	}
	return ret;
}


uint16_t ringbuf_blkXfer_2Array(ringbufHandler_t* src, uint16_t lower, uint16_t higher, int16_t* dst) {
	int16_t i, temp, ret;
	ret = 0;
	for(i=lower; i<higher; i++) {
		/*
		 * rewrite that to pop higher-lower samples off the ringbuff w/out using the pop function (update head and tail only once)
		 */
		ret = ringbuf_pop(src, &temp);
		dst[i] = temp;
	}
	return ret;
}


uint8_t ringbuf_blkXfer_2Array_noPop(ringbufHandler_t* src, uint16_t lower, uint16_t higher, int16_t* dst) {
	uint16_t i;
	uint16_t mask = higher-lower+1;
	if(ringbuf_getAvailableElements(src) > higher-lower-1) {
		for(i=0; i<higher-lower;i++) {
			dst[lower+i] = src->bufferPointer[++(src->tail)%mask];	//repositions the tail on its own
		}
		(src->tail)++;
		return 1;	//ok
	}
	return 0;	//sample dropped
}



void ringbuf_blkXfer_2ringbuf(int16_t* src, uint16_t lower, uint16_t higher, ringbufHandler_t * dst) {
	uint16_t i;
	for(i=lower; i<higher; i++) {
		ringbuf_push(dst, src[i]);
	}
}
