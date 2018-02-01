/* Universidad Galileo
 * Turing Research Lab
 * Julio E. Fajardo
 * Galileo Bionic Hand
 * CMSIS-DSP Application
 * Embedded Prostheses Controller
 * May-09-2017
 * finger.c
 */
 
#define ARM_MATH_CM4
 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "MK20D7.h"
#include "drivers.h"    
#include "finger.h"

const uint32_t fingers_mask[] = { 1<<4, 1<<2,  1<<3,
                                  1<<3, 1<<4,  1<<2,
                                  1<<7, 1<<13, 1<<12,
                                  1<<0, 1<<6,  1<<1
                                };

GPIO_Type *fingers_addr[]    = { PTC, PTC, PTC,
                                  PTD, PTD, PTD,
                                  PTD, PTA, PTA,
                                  PTD, PTD, PTC
                                };

void Finger_Close(uint8_t finger_m){
	if((finger_m>0)&&(finger_m<=6)){	
		fingers_addr[2*(finger_m-1)]->PSOR  |=  fingers_mask[2*(finger_m-1)];
		fingers_addr[2*(finger_m-1)+1]->PCOR  |=  fingers_mask[2*(finger_m-1)+1];
	}
}

void Finger_Open(uint8_t finger_m){
  if((finger_m>0)&&(finger_m<=6)){	
		fingers_addr[2*(finger_m-1)]->PCOR  |=  fingers_mask[2*(finger_m-1)];
		fingers_addr[2*(finger_m-1)+1]->PSOR  |=  fingers_mask[2*(finger_m-1)+1];
	}
}

void Finger_Stop(uint8_t finger_m){
  if((finger_m>0)&&(finger_m<=6)){	
		fingers_addr[2*(finger_m-1)]->PCOR  |=  fingers_mask[2*(finger_m-1)];
		fingers_addr[2*(finger_m-1)+1]->PCOR  |=  fingers_mask[2*(finger_m-1)+1];
	}
}

void Finger_Action(fingers *finger_f, uint8_t action, int16_t time_offset){
	if(action == CLOSE){
		switch(finger_f->state){
			case OPEN:{
				finger_f->state = CLOSE;
				Finger_Stop(finger_f->finger_m);
			} break;
			case WAITC:{
				finger_f->state = CLOSE;
				Finger_Stop(finger_f->finger_m);
			} break;
			case CLOSE:{
				arm_mean_q15(finger_f->buffer,SIZE,&finger_f->mean);
				if((finger_f->mean<finger_f->threshold)&&(finger_f->state == CLOSE)){
					finger_f->state = CLOSE;
					Finger_Close(finger_f->finger_m);
				}
				else { 
					finger_f->state = WAITO;
					Finger_Stop(finger_f->finger_m);
				}	
			} break;
			case WAITO:{
				finger_f->state = WAITO;
				finger_f->time_difference = finger_f->time_ms/4 + time_offset;
				Finger_Stop(finger_f->finger_m);
			} break;
		}
	} else{	//OPEN
		switch(finger_f->state){
			case OPEN:{
				if((finger_f->time_ms>finger_f->time_difference)&&(finger_f->state == OPEN)){
					finger_f->state = OPEN;
					Finger_Open(finger_f->finger_m);
				}
				else{
					finger_f->state = WAITC;
					finger_f->time_ms = 0;
					Finger_Stop(finger_f->finger_m);
				}
			} break;
			case WAITC:{
				finger_f->state = WAITC;
				Finger_Stop(finger_f->finger_m);
			} break;
			case CLOSE:{
				finger_f->state = OPEN;
				Finger_Stop(finger_f->finger_m);
			} break;
			case WAITO:{
				finger_f->state = OPEN;
				Finger_Stop(finger_f->finger_m);
			} break;
		}
	}
}

void Finger_Timing(fingers *finger_f){
	switch(finger_f->state){
		case OPEN:  finger_f->time_ms -= 1; break;
		case WAITC: finger_f->time_ms = 0; break;
		case CLOSE: finger_f->time_ms += 1; break;
		case WAITO: finger_f->time_ms += 0;
	}
}

void Finger_ActionTime(fingers *finger_f, uint8_t action, int16_t time_offset){
	if(action == CLOSE){
		switch(finger_f->state){
			case OPEN:{
				finger_f->state = CLOSE;
				Finger_Stop(finger_f->finger_m);
			} break;
			case WAITC:{
				finger_f->state = CLOSE;
				Finger_Stop(finger_f->finger_m);
			} break;
			case CLOSE:{
				if((finger_f->time_ms<finger_f->time_difference)&&(finger_f->state == CLOSE)){
					finger_f->state = CLOSE;
					Finger_Close(finger_f->finger_m);
				}
				else { 
					finger_f->state = WAITO;
					Finger_Stop(finger_f->finger_m);
				}	
			} break;
			case WAITO:{
				finger_f->state = WAITO;
				Finger_Stop(finger_f->finger_m);
			} break;
		}
	} else{	//OPEN
		switch(finger_f->state){
			case OPEN:{
				if((finger_f->time_ms>time_offset)&&(finger_f->state == OPEN)){
					finger_f->state = OPEN;
					Finger_Open(finger_f->finger_m);
				}
				else{
					finger_f->state = WAITC;
					finger_f->time_ms = 0;
					Finger_Stop(finger_f->finger_m);
				}
			} break;
			case WAITC:{
				finger_f->state = WAITC;
				Finger_Stop(finger_f->finger_m);
			} break;
			case CLOSE:{
				finger_f->state = OPEN;
				Finger_Stop(finger_f->finger_m);
			} break;
			case WAITO:{
				finger_f->state = OPEN;
				Finger_Stop(finger_f->finger_m);
			} break;
		}
	}
}

void Finger_Rotation(fingers *finger_f, uint8_t action){
}
