/*
 * codec.h
 *
 */

#ifndef CODEC_H_
#define CODEC_H_

/*
 * ----------------------------------------------------
 * 						INCLUDES
 * ----------------------------------------------------
 */
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include <stdint.h>
#include "I2C.h"
#include "engine.h"

/*
 * ----------------------------------------------------
 * 						DEFINES
 * ----------------------------------------------------
 */

/*
 * CODEC GPIO
 */
#define CODEC_I2S_SCK GPIO_PIN_13
#define CODEC_I2S_SOUT GPIO_PIN_15
#define CODEC_I2S_SIN GPIO_PIN_14
#define CODEC_I2S_LRCLK GPIO_PIN_12

#define CODEC_GPIO_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
#define CODEC_GPIO GPIOB

/*
 * CODEC Controller
 */
#define CODEC_I2S_CLK_ENABLE __HAL_RCC_SPI2_CLK_ENABLE

/*
 * CODEC Registers
 */
#define CODEC_ADDRESS 0x1a

#define W8731_NUM_REGS 10

#define CODEC_REG_LEFT_LINE_IN 0x00
#define CODEC_REG_RIGHT_LINE_IN 0x01
#define CODEC_REG_LEFT_HEADPHONES_OUT 0x02
#define CODEC_REG_RIGHT_HEADPHONES_OUT 0x03
#define CODEC_REG_ANALOGUE_ROUTING 0x04
#define CODEC_REG_DIGITAL_ROUTING 0x05
#define CODEC_REG_POWER_MANAGEMENT 0x06
#define CODEC_REG_DIGITAL_FORMAT 0x07
#define CODEC_REG_SAMPLE_RATE 0x08
#define CODEC_REG_ACTIVE 0x09
#define CODEC_REG_RESET 0x0f


#define CODEC_INPUT_0_DB 0x0017
#define CODEC_INPUT_MUTE 0x0080
#define CODEC_INPUT_UPDATE_BOTH 0x0100

#define CODEC_HEADPHONES_MUTE 0x0000

#define CODEC_ANALOGPATH_MICBOOST 0x0001
#define CODEC_ANALOGPATH_MUTEMIC 0x0002
#define CODEC_ANALOGPATH_MIC2ADC 0x0004
#define CODEC_ANALOGPATH_BYPASS 0x0008
#define CODEC_ANALOGPATH_DACSEL 0x0010
#define CODEC_ANALOGPATH_SIDETONE 0x0200

#define CODEC_DIGITALPATH_HPFDISABLE 0x0001
#define CODEC_DIGITALPATH_DEEMP_NONE 0x0000
#define CODEC_DIGITALPATH_DEEMP_32K 0x0002
#define CODEC_DIGITALPATH_DEEMP_44K 0x0004
#define CODEC_DIGITALPATH_DEEMP_48K 0x0006
#define CODEC_DIGITALPATH_DEEMP_DACMU 0x0008
#define CODEC_DIGITALPATH_DEEMP_HPOR 0x0010

#define CODEC_POWER_DOWN_LINE_IN 0x0001
#define CODEC_POWER_DOWN_MIC 0x0002
#define CODEC_POWER_DOWN_ADC 0x0004
#define CODEC_POWER_DOWN_DAC 0x0008
#define CODEC_POWER_DOWN_LINE_OUT 0x0010
#define CODEC_POWER_DOWN_OSCILLATOR 0x0020
#define CODEC_POWER_DOWN_CLOCK_OUTPUT 0x0040
#define CODEC_POWER_DOWN_EVERYTHING 0x0080

#define CODEC_PROTOCOL_MASK_MSB_FIRST 0x0000
#define CODEC_PROTOCOL_MASK_LSB_FIRST 0x0001
#define CODEC_PROTOCOL_MASK_PHILIPS 0x0002
#define CODEC_PROTOCOL_MASK_DSP 0x0003

#define CODEC_FORMAT_MASK_16_BIT 0x0000
#define CODEC_FORMAT_MASK_20_BIT 0x0004
#define CODEC_FORMAT_MASK_24_BIT 0x0008
#define CODEC_FORMAT_MASK_32_BIT 0x000c
#define CODEC_FORMAT_LR_SWAP 0x0020
#define CODEC_FORMAT_MASTER 0x0040
#define CODEC_FORMAT_SLAVE 0x0000
#define CODEC_FORMAT_INVERT_CLOCK 0x0080

#define CODEC_RATE_48K_48K (0x00 << 2)
#define CODEC_RATE_8K_8K (0x03 << 2)
#define CODEC_RATE_96K_96K (0x07 << 2)
#define CODEC_RATE_32K_32K (0x06 << 2)
#define CODEC_RATE_44K_44K (0x08 << 2)


/*
 * ----------------------------------------------------
 * 						GLOBAL VARS
 * ----------------------------------------------------
 */

DMA_HandleTypeDef hdma_i2s_rx;
DMA_HandleTypeDef hdma_i2s_tx;
I2S_HandleTypeDef I2SHandle;
I2S_HandleTypeDef I2S_extHandle;
uint16_t DMA_TX_buffer[AUDIO_IO_BUFFER_SIZE];
uint16_t DMA_RX_buffer[AUDIO_IO_BUFFER_SIZE];



/*
 * ----------------------------------------------------
 * 						PROTOTYPES
 * ----------------------------------------------------
 */

/*
 * This function only initializes the I2S Pins, the I2C ones are configured thru i2c_Config()
 */
void codec_ConfigGPIO(void);
void codec_ConfigAudioInterface();
void codec_writeRegister(uint8_t regAddr, uint16_t data);
void codec_writeRegisterBruteForce(uint8_t regAddr, uint16_t data);
void codec_ConfigCodec_48K();
void codec_start();
void codec_pause();
void codec_resume();
void codec_setInputGain(uint8_t channel, uint16_t amplification);

/*
 * These functions are the callbacks used as DMA transfer callbacks
 */
void i2s_Xfr_halfCplt_TX(DMA_HandleTypeDef * hdma);
void i2s_Xfr_cplt_TX(DMA_HandleTypeDef * hdma);
void i2s_Xfr_halfCplt_RX(DMA_HandleTypeDef * hdma);
void i2s_Xfr_cplt_RX(DMA_HandleTypeDef * hdma);

void I2S_putDataInReg(I2S_HandleTypeDef *hi2s, uint16_t data);
void I2S_clearRXNE(I2S_HandleTypeDef* I2SHandle);

void I2S_enableContr(I2S_HandleTypeDef *hi2s);
void I2S_disableContr(I2S_HandleTypeDef *hi2s);

/*
 * Optionnal call back, it's called every time a sample is dropped at the output stage
 */
__weak void sampleDropCallback(void);

#endif /* CODEC_H_ */
