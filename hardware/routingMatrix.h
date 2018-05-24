/*
 * routingMatrix.h
 *
 */

#ifndef ROUTINGMATRIX_H_
#define ROUTINGMATRIX_H_

#include "CVRouter.h"
#include "RGBLEDArray.h"
#include "buttonArray.h"

#define MATRIX_SWITCHUPDATE_DELAY 700

/*
 * Global variables
 */
LEDDriver_t U55, U54;


typedef struct {
	SPI_HandleTypeDef spiController;
	CVRouter_t router;
	buttonArray_t ba;
	RGBLEDArray_t la;
	uint32_t switchUpdateStartTime;
	uint8_t switchUpdateFlag;
} routingMatrix_t;

void routingMatrix_init(routingMatrix_t* matrix);
void routingMatrix_reset(routingMatrix_t* matrix);
/*
 *	Used by the UI state machine
 */
void routingMatrix_updateButtonStates(routingMatrix_t* matrix);
void routingMatrix_updateCVDestinations(routingMatrix_t* matrix);
void routingMatrix_syncLEDsToDestinations(routingMatrix_t* matrix);
void routingMatrix_forceSyncLEDsToDestinations(routingMatrix_t* matrix);

#endif /* ROUTINGMATRIX_H_ */
