/*
 * DAC.h
 *
 */

#ifndef DAC_H_
#define DAC_H_


/*
 * HAL include(s)
 */
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"

#define DAC_INSTANCE DAC1


/*
 * Global variables
 */
DAC_HandleTypeDef hdac_rollo;
DMA_HandleTypeDef hdma_rollo;

/*
 * DAC control functions
 */
void DAC_Init(void);
void DAC_GPIOConfig(void);
void DAC_Start(void);

void DAC_setValue(uint16_t value);

#endif /* DAC_H_ */
