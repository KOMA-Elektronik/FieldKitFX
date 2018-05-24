/*
 * codec.c
 *
 */


#include "codec.h"
#include "debug.h"
#include "stm32f3xx_it.h"

int16_t audio_input_buffer[AUDIO_IO_BUFFER_SIZE];
int16_t audio_output_buffer[AUDIO_IO_BUFFER_SIZE];

void codec_ConfigGPIO(void) {
	CODEC_GPIO_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = CODEC_I2S_SCK | CODEC_I2S_LRCLK | CODEC_I2S_SIN | CODEC_I2S_SOUT;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;	//same as GPIO_AF5_I2S2ext
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(CODEC_GPIO, &GPIO_InitStruct);
}

void codec_ConfigAudioInterface() {
	RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	RCC_PeriphCLKInitStruct.I2sClockSelection = RCC_I2SCLKSOURCE_EXT;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);


	CODEC_I2S_CLK_ENABLE();

	I2SHandle.Instance = SPI2;

	HAL_I2S_DeInit(&I2SHandle);

	__HAL_I2S_DISABLE(&I2SHandle);

	I2SHandle.Init.Mode = I2S_MODE_SLAVE_TX;
	I2SHandle.Init.Standard = I2S_STANDARD_PHILIPS;
	I2SHandle.Init.DataFormat = I2S_DATAFORMAT_16B_EXTENDED;
	I2SHandle.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
	I2SHandle.Init.AudioFreq = I2S_AUDIOFREQ_48K;
	I2SHandle.Init.CPOL = I2S_CPOL_LOW;
	I2SHandle.Init.ClockSource = I2S_CLOCK_EXTERNAL;
	I2SHandle.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;

	/*
	 * RX DMA Channel
	 */
	__HAL_RCC_DMA1_CLK_ENABLE();

	hdma_i2s_rx.Instance = DMA1_Channel4;
	hdma_i2s_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_i2s_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_i2s_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_i2s_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_i2s_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma_i2s_rx.Init.Mode = DMA_CIRCULAR;
	hdma_i2s_rx.Init.Priority = DMA_PRIORITY_HIGH;

	if (HAL_DMA_Init(&hdma_i2s_rx) != HAL_OK)	__asm("bkpt");

	__HAL_LINKDMA(&I2SHandle, hdmarx, hdma_i2s_rx);

	/* DMA1_Channel4_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

	/*
	 * TX DMA Channel
	 */
	hdma_i2s_tx.Instance = DMA1_Channel5;
	hdma_i2s_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_i2s_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_i2s_tx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_i2s_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_i2s_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma_i2s_tx.Init.Mode = DMA_CIRCULAR;
	hdma_i2s_tx.Init.Priority = DMA_PRIORITY_HIGH;

	if (HAL_DMA_Init(&hdma_i2s_tx) != HAL_OK)	__asm("bkpt");

	__HAL_LINKDMA(&I2SHandle, hdmatx, hdma_i2s_tx);

	/* DMA1_Channel5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);



	if (HAL_I2S_Init(&I2SHandle) != HAL_OK)
	{
		__asm("bkpt");
	}
}


void codec_start() {
	GPIO_InitTypeDef GPIOInitStruct;

	GPIOInitStruct.Pin = GPIO_PIN_8;
	GPIOInitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOInitStruct.Speed = GPIO_SPEED_MEDIUM;
	GPIOInitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &GPIOInitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	GPIOInitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD, &GPIOInitStruct);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
	GPIOInitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
	HAL_GPIO_Init(GPIOC,&GPIOInitStruct);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

	/*
	 * start the codec
	 */
	codec_writeRegister(CODEC_REG_ACTIVE, 0x01);

	while(HAL_GPIO_ReadPin(CODEC_GPIO, CODEC_I2S_LRCLK));
	while(!HAL_GPIO_ReadPin(CODEC_GPIO, CODEC_I2S_LRCLK));

	if(HAL_I2SEx_TransmitReceive_DMA(&I2SHandle, audio_output_buffer, audio_input_buffer, AUDIO_IO_BUFFER_SIZE) != HAL_OK){
		__asm("bkpt");
	}

	/*
	 * Assign the callbacks
	 */
	I2SHandle.hdmarx->XferCpltCallback = i2s_Xfr_cplt_RX;
	I2SHandle.hdmarx->XferHalfCpltCallback = i2s_Xfr_halfCplt_RX;
	I2SHandle.hdmatx->XferCpltCallback = i2s_Xfr_cplt_TX;
	I2SHandle.hdmatx->XferHalfCpltCallback = i2s_Xfr_halfCplt_TX;
	/*
	 * Enable Transfer Half Complete Interrupts
	 */
	hdma_i2s_rx.Instance->CCR |= (1 << 2);
	hdma_i2s_tx.Instance->CCR |= (1 << 2);
}

void codec_pause() {
	/*
	 * Pause the DMA transfer
	 */
	HAL_I2S_DMAPause(&I2SHandle);
	/*
	 * stop the codec
	 */
	codec_writeRegisterBruteForce(CODEC_REG_ACTIVE, 0x00);
}

void codec_resume() {
	/*
	 * start the codec
	 */
	codec_writeRegister(CODEC_REG_ACTIVE, 0x01);

	while(HAL_GPIO_ReadPin(CODEC_GPIO, CODEC_I2S_LRCLK));
	while(!HAL_GPIO_ReadPin(CODEC_GPIO, CODEC_I2S_LRCLK));

	/*
	 * resume DMA
	 */
	HAL_I2S_DMAResume(&I2SHandle);

}

/*
 * DMA ISR
 */
void DMA1_Channel4_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_i2s_rx);
}

void DMA1_Channel5_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_i2s_tx);
}


/*
 * DMA Callbacks
 */
void i2s_Xfr_halfCplt_RX(DMA_HandleTypeDef * hdma) {
	/*
	 * The DMA controller is now filling the second half of the DMA buffer, we can safely read the contents of the first half
	 * and push them in the audio_in_ringbuffer
	 */
	uint8_t i;

	for(i=0; i<AUDIO_IO_BUFFER_SIZE/4; i++) {
		user_audio_in_buffer.buffer[i] = audio_input_buffer[2*i];
		//ditch the RIGHT channel
	}
	audio_in_buffer_reset(&user_audio_in_buffer);
}

void i2s_Xfr_cplt_RX(DMA_HandleTypeDef * hdma) {
	/*
	 * The DMA controller is now filling the first half of the DMA buffer, we can safely read the contents of the second half
	 * and push them in the user in buffer
	 */

	uint8_t i;

	for(i=0; i<AUDIO_IO_BUFFER_SIZE/4; i++) {
		user_audio_in_buffer.buffer[i] = audio_input_buffer[(AUDIO_IO_BUFFER_SIZE/2) + 2*i];
		//ditch the RIGHT channel
	}
	audio_in_buffer_reset(&user_audio_in_buffer);
}

void i2s_Xfr_halfCplt_TX(DMA_HandleTypeDef * hdma) {
	/*
	 * The DMA controller is now reading the second half of the DMA buffer, we can transfer the contents of the user buffer
	 * to the first half of the DMA buffer
	 */
	uint8_t i;

	// check for sample drops
	if(!audio_out_buffer_isFull(&user_audio_out_buffer)) {
		sampleDropCallback();
	}
	for(i=0; i<AUDIO_IO_BUFFER_SIZE/4; i++) {
		audio_output_buffer[2*i] = user_audio_out_buffer.buffer[i];
		audio_output_buffer[2*i+1] = 0;	//we don't use the RIGHT channel
	}
	audio_out_buffer_reset(&user_audio_out_buffer);
}

void i2s_Xfr_cplt_TX(DMA_HandleTypeDef * hdma) {
	/*
	 * The DMA controller is now reading the first half of the DMA buffer, we can transfer the contents of the user buffer
	 * to the second half of the DMA buffer
	 */
	uint8_t i;

	// check for sample drops
	if(!audio_out_buffer_isFull(&user_audio_out_buffer)) {
		sampleDropCallback();
	}
	for(i=0; i<AUDIO_IO_BUFFER_SIZE/4; i++) {
		audio_output_buffer[(AUDIO_IO_BUFFER_SIZE/2) + 2*i] = user_audio_out_buffer.buffer[i];
		audio_output_buffer[(AUDIO_IO_BUFFER_SIZE/2) + (2*i+1)] = 0;	//we don't use the RIGHT channel
	}
	audio_out_buffer_reset(&user_audio_out_buffer);
}



void codec_writeRegister(uint8_t regAddr, uint16_t data) {
	uint8_t temp[2];
	temp[0] = ((regAddr << 1) & 0xfe) | ((data >> 8) & 0x01);
	temp[1] = data;

	if(HAL_I2C_Master_Transmit(&I2c2Handle, (uint16_t) (CODEC_ADDRESS << 1), temp, 2, I2C_TIMEOUT_VAL) != HAL_OK)
		__asm("bkpt");
}

void codec_writeRegisterBruteForce(uint8_t regAddr, uint16_t data){
	uint8_t temp[2];
	temp[0] = ((regAddr << 1) & 0xfe) | ((data >> 8) & 0x01);
	temp[1] = data;
	HAL_I2C_Master_Transmit(&I2c2Handle, (uint16_t) (CODEC_ADDRESS << 1), temp, 2, I2C_TIMEOUT_VAL);
	while(HAL_I2C_GetState(&I2c2Handle) != HAL_I2C_STATE_READY){

	}
}

void codec_ConfigCodec_48K() {
	// Reset the codec
	codec_writeRegister(CODEC_REG_RESET, 0);

	// Do not activate the codec just yet
	codec_writeRegister(CODEC_REG_ACTIVE, 0x00);

	// Configure L&R inputs
	codec_writeRegister(CODEC_REG_LEFT_LINE_IN, CODEC_INPUT_0_DB);
	codec_writeRegister(CODEC_REG_RIGHT_LINE_IN, CODEC_INPUT_0_DB);	//mute the right input channel

	// Configure L&R headphone outputs
	codec_writeRegister(CODEC_REG_LEFT_HEADPHONES_OUT, CODEC_HEADPHONES_MUTE);
	codec_writeRegister(CODEC_REG_RIGHT_HEADPHONES_OUT, CODEC_HEADPHONES_MUTE);

	// Configure analog routing
	codec_writeRegister(CODEC_REG_ANALOGUE_ROUTING, CODEC_ANALOGPATH_MUTEMIC | CODEC_ANALOGPATH_DACSEL);

	// Configure digital routing
	codec_writeRegister(CODEC_REG_DIGITAL_ROUTING, CODEC_DIGITALPATH_DEEMP_NONE);	//hmmmm, no deemphasis ?

	// Configure power management, let the oscillator powered
	codec_writeRegister(CODEC_REG_POWER_MANAGEMENT, CODEC_POWER_DOWN_MIC);	//turn the mic off

	// Configure the format
	codec_writeRegister(CODEC_REG_DIGITAL_FORMAT, CODEC_FORMAT_MASK_16_BIT | CODEC_FORMAT_MASTER | CODEC_PROTOCOL_MASK_PHILIPS);

	// Configure the sample rate
	codec_writeRegister(CODEC_REG_SAMPLE_RATE, CODEC_RATE_48K_48K);
}

void codec_setInputGain(uint8_t channel, uint16_t amplification){
	static uint16_t value = 0;
	static uint32_t write_counter = 0;
	static uint32_t previous_time = 0;
	static uint32_t time = 0;
	time = HAL_GetTick();
	if((value != (amplification & 0x1F))&&(time - previous_time > 100)){
		previous_time = time;
		write_counter++;
		//limit to 5bits
		value = amplification & 0x1F;
		codec_writeRegisterBruteForce(channel, amplification & 0x1F);
	}
}

__weak void sampleDropCallback(void) {
//	__asm("bkpt");
}

