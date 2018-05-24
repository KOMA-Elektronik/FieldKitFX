/*
 * memory.c
 *
 */



#include "memory.h"


void RAM_init(RAMChip_t* RAMChip) {

	RAMChip->spiController = &memorySPIController;
	RAMChip->CSGPIOPort = GPIOA;
	RAMChip->CSPinNumber = GPIO_PIN_15;

	/*
	 * First setup the Chip select Line
	 */
	GPIO_InitTypeDef GPIOStruct;

	GPIOStruct.Pin = RAMChip->CSPinNumber;
	GPIOStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOStruct.Pull = GPIO_NOPULL;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(RAMChip->CSGPIOPort, &GPIOStruct);

	RAM_setCS(RAMChip, HIGH);

	//initialize the MODE register
	uint8_t temp[2];

	temp[0] = RAM_COMMAND_WRMR;
	temp[1] = 64;

	RAM_setCS(RAMChip, LOW);
	HAL_SPI_Transmit(RAMChip->spiController, temp, 2, 100);
	RAM_setCS(RAMChip, HIGH);

}

void Memory_initSPI() {
	__HAL_RCC_SPI3_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	memorySPIController.Instance = SPI3;

	HAL_SPI_DeInit(&memorySPIController);

	memorySPIController.Init.Mode = SPI_MODE_MASTER;		//Master mode
	memorySPIController.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;	//32MHz/2 => 16.5Mbps
	memorySPIController.Init.Direction = SPI_DIRECTION_2LINES;		//2 way communication
	memorySPIController.Init.DataSize = SPI_DATASIZE_8BIT;		//8 bit transmission
	memorySPIController.Init.FirstBit = SPI_FIRSTBIT_MSB;		//MSB first
	memorySPIController.Init.CLKPolarity = SPI_POLARITY_LOW;		//clock idle low
	memorySPIController.Init.CLKPhase	= SPI_PHASE_1EDGE;		//sampling on rising edge
	memorySPIController.Init.NSS = SPI_NSS_SOFT;		//Software NSS
	memorySPIController.Init.NSSPMode = SPI_NSS_PULSE_DISABLED;
	memorySPIController.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;		//No CRC calculation
	memorySPIController.Init.TIMode = SPI_TIMODE_DISABLE;

	HAL_SPI_Init(&memorySPIController);

	/*
	 * SPI GPIO setup
	 */
	GPIO_InitTypeDef GPIOStruct;

	GPIOStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
	GPIOStruct.Mode = GPIO_MODE_AF_PP;
	GPIOStruct.Speed = GPIO_SPEED_HIGH;
	GPIOStruct.Pull = GPIO_PULLDOWN;
	GPIOStruct.Alternate = GPIO_AF6_SPI3;


	HAL_GPIO_Init(GPIOC, &GPIOStruct);

}

void Memory_flushSPIFIFO(SPI_HandleTypeDef* spiController) {
	HAL_SPIEx_FlushRxFifo(spiController);
}


void RAM_setCS(RAMChip_t* RAMChip, uint8_t state) {
	if(state)
		HAL_GPIO_WritePin(RAMChip->CSGPIOPort, RAMChip->CSPinNumber, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(RAMChip->CSGPIOPort, RAMChip->CSPinNumber, GPIO_PIN_RESET);
}


void RAM_writeCell(RAMChip_t* RAMChip, uint32_t address, uint8_t data) {
	uint8_t temp[5];

	address &= 0x00ffffff;	//to discard the MSByte of the 32bit number

	temp[0] = RAM_COMMAND_WRITE;
	temp[1] = address >> 16;	//the MSByte of the 24bit address
	temp[2] = address >> 8;
	temp[3] = address;
	temp[4] = data;

	RAM_setCS(RAMChip, LOW);
	HAL_SPI_Transmit(RAMChip->spiController, temp, 5, 100);
	RAM_setCS(RAMChip, HIGH);
}

uint8_t RAM_readCell(RAMChip_t* RAMChip, uint32_t address) {
	uint8_t tempW[5];
	uint8_t recvVal[5] = {0xff};

	address &= 0x00ffffff;	//to discard the MSByte of the 32bit number

	tempW[0] = RAM_COMMAND_READ;
	tempW[1] = address >> 16;	//the MSByte of the 24bit address
	tempW[2] = address >> 8;
	tempW[3] = address;
	tempW[4] = 0xff;

	Memory_flushSPIFIFO(RAMChip->spiController);

	RAM_setCS(RAMChip, LOW);
	HAL_SPI_TransmitReceive(RAMChip->spiController, tempW, recvVal, 5, 100);
	RAM_setCS(RAMChip, HIGH);

	return recvVal[4];
}

void RAM_writeArray(RAMChip_t* RAMChip, uint32_t startAddress, uint8_t* data, uint32_t size) {
	uint8_t temp[4];

	startAddress &= 0x00ffffff;	//to discard the MSByte of the 32bit number

	temp[0] = RAM_COMMAND_WRITE;
	temp[1] = startAddress >> 16;	//the MSByte of the 24bit address
	temp[2] = startAddress >> 8;
	temp[3] = startAddress;

	RAM_setCS(RAMChip, LOW);
	HAL_SPI_Transmit(RAMChip->spiController, temp, 4, 100);
	HAL_SPI_Transmit(RAMChip->spiController, data, size, 100);
	RAM_setCS(RAMChip, HIGH);
}

void RAM_readArray(RAMChip_t* RAMChip, uint32_t startAddress, uint8_t* data, uint32_t size) {
	uint8_t tempW[4];

	startAddress &= 0x00ffffff;	//to discard the MSByte of the 32bit number

	tempW[0] = RAM_COMMAND_READ;
	tempW[1] = startAddress >> 16;	//the MSByte of the 24bit address
	tempW[2] = startAddress >> 8;
	tempW[3] = startAddress;

	RAM_setCS(RAMChip, LOW);
	HAL_SPI_Transmit(RAMChip->spiController, tempW, 4, 100);
	Memory_flushSPIFIFO(RAMChip->spiController);
	HAL_SPI_Receive(RAMChip->spiController, data, size, 100);
	RAM_setCS(RAMChip, HIGH);
}

void RAM_readFrame(RAMChip_t* RAMChip, uint16_t frameNumber, frame_t* frame) {
	uint8_t buffer[FRAME_BYTECOUNT];
	uint8_t i, block_counter;

	//calculate the address to start reading at
	//get 25 bytes from that address
	RAM_readArray(RAMChip, frameNumber*FRAME_BYTECOUNT, buffer, FRAME_BYTECOUNT);
	//format the bytes to make a frame (populate the blocks)
	for(block_counter = 0; block_counter<BLOCKS_PER_FRAME; block_counter++) {
		frame->samples[block_counter].nibbleByte = buffer[block_counter*BLOCK_SIZE];
		frame->samples[block_counter].lesser_samples[0] = buffer[block_counter*BLOCK_SIZE+1];
		frame->samples[block_counter].lesser_samples[1] = buffer[block_counter*BLOCK_SIZE+2];
	}
}


void RAM_writeFrame(RAMChip_t* RAMChip, uint16_t frameNumber, frame_t* frame) {
	uint8_t buffer[FRAME_BYTECOUNT];
	uint8_t i, block_counter;

	//format the bytes to make a frame (populate the blocks)
	for(block_counter = 0; block_counter<BLOCKS_PER_FRAME; block_counter++) {
		buffer[block_counter*BLOCK_SIZE] = frame->samples[block_counter].nibbleByte;
		buffer[block_counter*BLOCK_SIZE+1] = frame->samples[block_counter].lesser_samples[0];
		buffer[block_counter*BLOCK_SIZE+2] = frame->samples[block_counter].lesser_samples[1];
	}

	//calculate the address to start reading at
	//write the 25 bytes of the buffer array from that address
	RAM_writeArray(RAMChip, frameNumber*FRAME_BYTECOUNT, buffer, FRAME_BYTECOUNT);
}

void RAM_RWError() {
	__asm("bkpt");
}
