/*
 * main.h
 *
 */

#ifndef MAIN_H_
#define MAIN_H_

/*
 * System clock initialization using the external Xtal, turns on the PLLs and sets the peripheral buses clocks
 * Also configures the SysTick to increment every millisecond
 *
 * after the execution of that function, the MCU runs at 72MHz
 */
void systemClock_Config_HSE(void);

/*
* Initializes the internal high speed crystal (HSI, 8MHz) and make the whole processor run at 64MHz
*
* Sysclk : 64MHz
* APB1 Periph clk : 32MHz
* APB1 Timer clk : 64MHz
* APB2 Periph/Timer clk : 64MHz
*
*/
void systemClock_Config_HSI(void);


/*
 * small utils
 */
void enableAllGPIOs(void);

#endif /* MAIN_H_ */
