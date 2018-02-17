/* Universidad Galileo
 * Turing Research Lab
 * Julio E. Fajardo
 * Galileo Bionic Hand
 * CMSIS-DSP Application
 * Embedded Prostheses Controller
 * May-09-2017
 * finger.h
 */
 
#ifndef FINGER_H_
#define FINGER_H_

#include <stdint.h>
#include <stdlib.h>
#include "arm_math.h"                   // ARM::CMSIS:DSP

#define 		SIZE     50
#define 		ESIZE    25

#define			OPEN      0
#define			WAITC     1
#define			CLOSE     2
#define			WAITO     3

#define			POWER     0
#define			POINT     1
#define			PINCH     2
#define			HOOK      3
#define			LATERAL   4
#define			PEACE     5
#define 		REST      6

typedef struct finger{
	uint8_t state;
	uint8_t finger_m;
	uint32_t time_ms;	
	uint32_t time_difference;
	q15_t mean;
	q15_t threshold;
	q15_t buffer[SIZE];
	} fingers;

void Finger_Close(uint8_t finger_m);
void Finger_Open(uint8_t finger_m);
void Finger_Stop(uint8_t finger_m);
void Finger_Action(fingers * finger_f, uint8_t action, int16_t time_offset);
void Finger_Timing(fingers * finger_f);
void Finger_ActionTime(fingers *finger_f, uint8_t action, int16_t time_offset);
void Finger_Rotation(fingers *finger_f, uint8_t action);
void Hand_Action(uint8_t hand_action);

#endif /* DRIVERS_H_ */
