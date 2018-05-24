/*
 * memory.h
 *
 */

#ifndef MEMORY_H_
#define MEMORY_H_


#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include <stdint.h>
#include "frame.h"

#include "23LC1024.h"

/*
 * Pin state
 */
#define HIGH 1
#define LOW 0


/*
 * memory spi controller
 */
SPI_HandleTypeDef memorySPIController;


/*
 * General SPI functions
 */
void Memory_initSPI();
void Memory_flushSPIFIFO();

/*
 * RAM chip structure
 */
typedef struct {
	SPI_HandleTypeDef* spiController;
	/*
	 * chip select related parameters
	 */
	GPIO_TypeDef* CSGPIOPort;
	uint16_t CSPinNumber;
} RAMChip_t;

/*
 * RAM init, read and write functions
 */
void RAM_init(RAMChip_t* RAMChip);
void RAM_setCS(RAMChip_t* RAMChip, uint8_t state);
void RAM_writeCell(RAMChip_t* RAMChip, uint32_t address, uint8_t data);
uint8_t RAM_readCell(RAMChip_t* RAMChip, uint32_t address);
void RAM_writeArray(RAMChip_t* RAMChip, uint32_t startAddress, uint8_t* data, uint32_t size);
void RAM_readArray(RAMChip_t* RAMChip, uint32_t startAddress, uint8_t* data, uint32_t size);
void RAM_readFrame(RAMChip_t* RAMChip, uint16_t frameNumber, frame_t* frame);
void RAM_writeFrame(RAMChip_t* RAMChip, uint16_t frameNumber, frame_t* frame);

void RAM_RWError();

#endif /* MEMORY_H_ */
