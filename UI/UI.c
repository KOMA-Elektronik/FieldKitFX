/*
 * UI.c
 *
 */

#include "UI.h"

/*
 * Initialize all the parts necessary to control and process the UI.
 */

void UI_init(void) {
	FXSelector_init();
	routingMatrix_init(&CVmatrix);
	rolloMUX_init();
	routingMatrix_reset(&CVmatrix);
	loopButton_init(&loopButton, &U54);
	rolloSelector_init();

	TIM_ClockConfigTypeDef clockSourceStruct;

	__HAL_RCC_TIM2_CLK_ENABLE();
	UITimer.Instance = TIM2;
	UITimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	UITimer.Init.Period = 65535;
	UITimer.Init.Prescaler = 72;	//1MHz update rate
	UITimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	UITimer.Init.RepetitionCounter = 0;

	clockSourceStruct.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&UITimer, &clockSourceStruct);

	if(HAL_TIM_Base_Init(&UITimer) != HAL_OK) {
		__asm("bkpt");
	}

	HAL_TIM_Base_Start(&UITimer);
}

/*
 * This function is called in the main loop. It executes the different tasks of
 * the UI according to the current state of the state machine.
 */
void UI_render(void) {
	switch(current_UI_state) {
	case UI_matrixButtons_update:
		if(__HAL_TIM_GET_COUNTER(&UITimer) > MATRIX_REFRESH_PERIOD) {
			matrixRefreshFlag = 1;
			//reset the timer value
			UITimer.Instance->CNT = 0;
			routingMatrix_updateButtonStates(&CVmatrix);
			if(buttonArray_activeCombination(&(CVmatrix.ba)) == CALIBRATION_COMBINATION){
				buttonArray_resetCombinations(&(CVmatrix.ba));
				FXSelector_switchToCalibration();
				//we don't want to update the matrix
				matrixRefreshFlag = 0;
			}
		}
		break;
	case UI_matrixRouting_update:
		if(matrixRefreshFlag){
			routingMatrix_updateCVDestinations(&CVmatrix);
		}
		break;
	case UI_matrixLEDs_update:
		if(matrixRefreshFlag){
			routingMatrix_syncLEDsToDestinations(&CVmatrix);
		}
		break;
	case UI_rolloMUX_update:
		if(matrixRefreshFlag){
			rollodecks_update();
			matrixRefreshFlag = 0;
		}
		break;
	case UI_loopButton_update:
		loopButton_updateStates(&loopButton);
		if(FXSelector_selectedFX() == FX_LOOPER) {
			switch(looper.state) {
			case ARMED:
				looper.previousState = ARMED;
				if(loopButton_risingEdge(&loopButton)) {
					looper.flag = 0;
					looper.framePointer = 0;
					looper.state = RECORD;
					looper.previousState = ARMED;
					loopButton_changeColor(&loopButton, LOOPLED_RECORD);
					loopButton_changeIntensity(&loopButton, LOOPLED_HIGH_INTENSITY);
				}
				break;

			case RECORD:
				looper.previousState = RECORD;
				if(loopButton_fallingEdge(&loopButton) || looper.framePointer >= MAX_FRAME_NUM){
					looper.state = PLAYBACK;
					looper.previousState = RECORD;
					looper.endFramePosition = looper.framePointer - 1;
					looper.framePointer = 0;
					looper.doFadeOut = FRAME_NUMBER_FADE_IN;
					loopButton_changeIntensity(&loopButton, LOOPLED_NORMAL_INTENSITY);
					loopButton_changeColor(&loopButton, LOOPLED_PLAYBACK);
				}
				break;

			case PLAYBACK:
				looper.previousState = PLAYBACK;
				if(loopButton_isLow(&loopButton)) {
					looper.flag = 1;
				}
				if(looper.endFramePosition >= BLINK_FRAME_THRESHOLD+BLINK_GUARD_INTERVAL) {
					if(looper.framePointer >= looper.endFramePosition - BLINK_FRAME_THRESHOLD) {
						loopButton_changeIntensity(&loopButton, 0);
					}
					else if(looper.framePointer == 0) {
						loopButton_changeIntensity(&loopButton, LOOPLED_NORMAL_INTENSITY);
					}
				}
				if(loopButton_risingEdge(&loopButton)) {
					looper.state = OVERDUB;
					looper.previousState = PLAYBACK;
					looper.doOverdubFadeIn = FRAME_NUMBER_FADE_IN;
					loopButton_changeColor(&loopButton, LOOPLED_OVERDUB);
					loopButton_changeIntensity(&loopButton, LOOPLED_NORMAL_INTENSITY);
				}
				break;

			case OVERDUB:
				looper.previousState = OVERDUB;
				if(loopButton_fallingEdge(&loopButton)) {
					if(loopButton_wasShortPress(&loopButton)) {
						looper.state = ERASE;
						looper.previousState = OVERDUB;
						loopButton_changeColor(&loopButton, LOOPLED_ERASE);
						loopButton_changeIntensity(&loopButton, LOOPLED_NORMAL_INTENSITY);
					}
					else {
						looper.state = OVERDUB;//the looper switches to playback after the fadeout
						looper.doOverdubFadeOut = FRAME_NUMBER_FADE_IN;
						looper.previousState = OVERDUB;
						loopButton_changeColor(&loopButton, LOOPLED_PLAYBACK);
						loopButton_changeIntensity(&loopButton, LOOPLED_NORMAL_INTENSITY);
					}
				}
				break;

			case ERASE:
				looper.previousState = ERASE;
				if(looper.framePointer >= looper.endFramePosition) {
					looper.state = ARMED;
					looper.previousState = ERASE;
					loopButton_changeColor(&loopButton, LOOPLED_ARMED);
				}
				break;

			default:
				break;
			}
		}
		if(FXSelector_justSwitchedTo(FX_FREQ_SHIFT)){
			loopButton_changeColor(&loopButton, LOOPLED_OFF);
		}
		break;
	case UI_modeSwitches_update:
		FXSelector_update();
		if(FXSelector_justSwitchedTo(FX_LOOPER)) {
			looper.state = ARMED;
			loopButton_changeColor(&loopButton, LOOPLED_ARMED);
			loopButton_changeIntensity(&loopButton, LOOPLED_NORMAL_INTENSITY);
		}
		break;
	case UI_tresholdcv_update:
		rolloSelector_update();

		if(rolloSelector_justSwitchedTo(ROLLO_SEQ)) {
			sequencer_ui_init();
		}
		else if(rolloSelector_justSwitchedTo(ROLLO_ENV)) {
			ADSR_ui_init();
		}
		if(rolloSelector_selectedRollo(ROLLO_SEQ)) {
			sequencer_processCV((ADC_getThresholdPot() >> 2));
			sequencer_setMode(CVRouter_getSource(&(CVmatrix.router), SEQUENCER_CV_DESTINATION));
		}
		else {
			ADSR_processADSRCV(&rollo_env, rolloCV_values[0] >> 2, rolloCV_values[1] >> 2, rolloCV_values[2] >> 2, rolloCV_values[3] >> 2);
			ADSR_processThresholdPot(&rollo_env, (ADC_getThresholdPot() >> 2));
			if(rollo_env.process_ui){
				ADSR_setLengthThreshold(&rollo_env);
				ADSR_setMode(&rollo_env, ADC_getThresholdPot() >> 2, CVRouter_getSource(&(CVmatrix.router), SEQUENCER_CV_DESTINATION));
				ADSR_setA(&rollo_env);
				ADSR_setD(&rollo_env);
				ADSR_setS(&rollo_env);
				ADSR_setR(&rollo_env);
				rollo_env.process_ui = 0;
			}
		}
		break;
	}
	//update the state
	current_UI_state++;
	current_UI_state = current_UI_state%NUMBEROFUISTATES;
}

/*
 * This inits the Calibration-Mode once the correspind button combination has been pressed.
 */
void UI_initCalibration(void){
	current_UI_state_Calibration = 0;
	UI_renderMagnitudeinit();
}

/*
 * This renders the UI in calibration mode.
 */
void UI_renderCalibration(void){
	switch(current_UI_state_Calibration){
	case UI_matrixButtons_update_Calibration:
		//check if the correct amount of time has elapsed
		if(__HAL_TIM_GET_COUNTER(&UITimer) > MATRIX_REFRESH_PERIOD) {
			//reset the timer value
			UITimer.Instance->CNT = 0;
			routingMatrix_updateButtonStates(&CVmatrix);
			if(buttonArray_activeCombination(&(CVmatrix.ba)) == CALIBRATION_COMBINATION){
				//reset the active Combination
				buttonArray_resetCombinations(&(CVmatrix.ba));
				//switch back to the effect depending on the actual switch position
				FXSelector_update();
				//reinit Brightness settings
				RGBLEDArray_init(&(CVmatrix.la), &U54, &U55);
				//reset all CV routings
				CVRouter_disableAllRoutings(&(CVmatrix.router));
				//sync LEDs to actual routing
				routingMatrix_forceSyncLEDsToDestinations(&CVmatrix);
			}
		}
		break;
	case UI_renderMagnitude_Calibration:
		UI_renderMagnitude(magnitudeTracker_getMax());
		break;
	}
	//update the state
	current_UI_state_Calibration++;
	current_UI_state_Calibration = current_UI_state_Calibration%NUMBEROFUISTATESCALIBRATION;
}

void UI_renderMagnitudeinit(void){
//	first 9 leds green
	for(uint8_t i=0;i<9;i++){
		RGBLEDArray_setLEDColor(&CVmatrix.la,i,0,127,0);
	}
//	10th led yellow
	RGBLEDArray_setLEDColor(&CVmatrix.la,9,0xff, 0x10, 0);
//	11th led red
	RGBLEDArray_setLEDColor(&CVmatrix.la,10,0xff, 0,0);
//	turn all leds off
	RGBLEDArray_setGlobalIntensity(&CVmatrix.la,0);
}

void UI_renderMagnitude(uint16_t magnitude){
	volatile uint16_t scaledMagnitude = magnitude / 2979; //fit it into the 11 leds <-- scaledMagnitude_max = 10
	static uint16_t previousMagnitude = 0;
	if(scaledMagnitude != previousMagnitude){
		for(uint8_t i = 0;i<=scaledMagnitude;i++){
			RGBLEDArray_setIntensity(&CVmatrix.la,i,0x04);
		};
		for(uint8_t i = scaledMagnitude+1;i<11;i++){
			RGBLEDArray_setIntensity(&CVmatrix.la,i,0x00);
		};
		previousMagnitude = scaledMagnitude;
	}
}
