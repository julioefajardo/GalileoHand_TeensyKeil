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

void Finger_Close(uint8_t finger_m);
void Finger_Open(uint8_t finger_m);
void Finger_Stop(uint8_t finger_m);
void Finger_Timing(uint8_t *state, uint32_t * time_ms);
void Finger_Action(fingers * finger_f, uint8_t action);

#endif /* DRIVERS_H_ */
