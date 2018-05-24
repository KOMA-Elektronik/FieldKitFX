/*
 * Xfader.h
 *
 */

#ifndef XFADER_H_
#define XFADER_H_


/*
 * Structure definition
 */
typedef struct {
	float lambda;
} Xfader_t;

/*
 * Functions definitions
 */
float Xfader_process(Xfader_t* xfader, float in1, float in2);

#endif /* XFADER_H_ */
