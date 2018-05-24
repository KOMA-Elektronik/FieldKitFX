/*
 * DCRemover.h
 *
 * A single pole high pass.
 */

#ifndef DCREMOVER_H_
#define DCREMOVER_H_

/*
 * Structure definition
 */
typedef struct {
	float pole;
	float old_x;
	float old_y;
} DCRemover_t;

/*
 * Function definitions
 */
void DC_remover_init(DCRemover_t* remover, float pole);
float DC_remover_process(DCRemover_t* remover, float inputSample);

#endif /* DCREMOVER_H_ */
