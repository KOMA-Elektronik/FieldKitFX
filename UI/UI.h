/*
 * UI.h
 *
 * UI takes care of handling any user input and displaying the current settings.
 *
 * It's organized in a fairly simple state machine. UI_render is called in the main loop
 * and the task according to the current state is executed. Some of those tasks are dependent
 * on the currently selected effect.
 *
 * In calibration mode (currently only used to set the input gain), the ui switches over to
 * a seperate state machine.
 *
 * The UI consists of the following parts:
 * - the Effect-Selectswitch (either Frequencyshifter or Looper) : FXSelector
 * - the CV-Routingmatrix, including all Buttons, LEDs and internal Hardware: routingMatrix
 * - the Multiplexer used to switch between the four Pots of the Roll-O-Decks: rolloMux
 * - the Looper-Button: loopButton
 * - the Modeselector-Switch of the Roll-O-Decks: rolloSelector
 *
 */


#ifndef UI_H_
#define UI_H_

#include "routingMatrix.h"
#include "rollodecks.h"
#include "FXSelector_switch.h"
#include "rolloSelector_switch.h"
#include "codec.h"
#include "loopButton.h"
#include "looper.h"
#include "sequencer.h"
#include "adsr.h"
#include "Gate.h"
#include "magnitudeTracker.h"


/*
 * Refresh period of the CV-Matrix.
 */
#define MATRIX_REFRESH_PERIOD 10000	//1MHz/10000 = 100Hz

/*
 * States of the default state machine.
 */
#define NUMBEROFUISTATES 7

typedef enum {
	UI_matrixButtons_update = 0,
	UI_matrixRouting_update,
	UI_matrixLEDs_update,
	UI_rolloMUX_update,
	UI_loopButton_update,
	UI_modeSwitches_update,
	UI_tresholdcv_update
} UI_state;

/*
 * States of the calibration state machine.
 */

#define NUMBEROFUISTATESCALIBRATION 2

typedef enum {
	UI_matrixButtons_update_Calibration = 0,
	UI_renderMagnitude_Calibration
} UI_state_Calibration;

UI_state current_UI_state;
UI_state_Calibration current_UI_state_Calibration;
TIM_HandleTypeDef UITimer;
routingMatrix_t CVmatrix;
uint8_t matrixRefreshFlag;

void UI_init(void);
void UI_render(void);
void UI_initCalibration(void);
void UI_renderCalibration(void);
void UI_renderMagnitudeinit(void);
void UI_renderMagnitude(uint16_t magnitude);


#endif /* UI_H_ */
