/*
 * CVRouter.h
 *
 */

#ifndef CVROUTER_H_
#define CVROUTER_H_

#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_spi.h"
#include "stdint.h"

/*
 * Defines
 */
#define MATRIX_SPI_TIMEOUT 100
#define MATRIX_LOADPORT GPIOF
#define MATRIX_LOADPIN 2
#define MATRIX_SPI_CONTROLLER SPI1

#define CV_SOURCE_1 0x01
#define CV_SOURCE_2 0x02
#define CV_SOURCE_3 0x04
#define CV_SOURCE_4 0x08
#define CV_SOURCE_REFERENCE 0x10

/*
 * Global variables
 */
SPI_HandleTypeDef controller;


/*
 * Structure definition
 */
typedef struct {
	SPI_HandleTypeDef* spi_controller;
	uint8_t state[12];
} CVRouter_t;


/*
 * Functions
 */
void CVRouter_init(CVRouter_t* router);
void CVRouter_disableAllRoutings(CVRouter_t* router);
void CVRouter_setDestination(CVRouter_t* router, uint8_t src, uint8_t dst);
void CVRouter_cycleSource(CVRouter_t* router, uint8_t dst);
uint8_t CVRouter_getSource(CVRouter_t* router, uint8_t dst) ;
void CVRouter_setSwitches(CVRouter_t* router);

#endif /* CVROUTER_H_ */
