/*
 * ringbuffer.h
 *
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

/* --- Includes --- */
#include <stdint.h>

/* --- Ringbuffer handler definition --- */
typedef struct ringbufHandler_t {
	int16_t* bufferPointer;
	uint16_t head, tail;	//head : where you write; tail : where you read 
	uint16_t size;
	uint16_t mask;
} ringbufHandler_t;

/* --- Prototypes --- */
void ringbuf_reset(ringbufHandler_t *buffer);
uint16_t ringbuf_isFull(ringbufHandler_t *buffer);
uint16_t ringbuf_isEmpty(ringbufHandler_t *buffer);
uint16_t ringbuf_getUsedElements(ringbufHandler_t *buffer);
uint16_t ringbuf_getAvailableElements(ringbufHandler_t *buffer);
uint16_t ringbuf_push(ringbufHandler_t *buffer, int16_t data);
uint16_t ringbuf_pop(ringbufHandler_t *buffer, int16_t *data);

uint16_t ringbuf_blkXfer_2Array(ringbufHandler_t* src, uint16_t lower, uint16_t higher, int16_t* dst);
uint8_t ringbuf_blkXfer_2Array_noPop(ringbufHandler_t* src, uint16_t lower, uint16_t higher, int16_t* dst);
void ringbuf_blkXfer_2ringbuf(int16_t* src, uint16_t lower, uint16_t higher, ringbufHandler_t * dst);


#endif /* RINGBUFFER_H_ */
