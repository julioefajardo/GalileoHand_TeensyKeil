/* Universidad Galileo
 * Turing Research Lab
 * Julio E. Fajardo
 * Galileo Bionic Hand
 * CMSIS-DSP Application
 * Embedded Prostheses Controller
 * May-09-2017
 * main.c
 */

#define ARM_MATH_CM4

#include "MK20D7.h"                     					// Device header
#include "arm_math.h"                   					// ARM::CMSIS:DSP
#include <stdlib.h>
#include "drivers.h"

const uint32_t fingers_mask[] = { 1<<4, 1<<2,  1<<3,
                                  1<<3, 1<<4,  1<<2,
                                  1<<7, 1<<13, 1<<12,
                                  1<<0, 1<<6,  1<<1
                                };

GPIO_Type * fingers_addr[]    = { PTC, PTC, PTC,
                                  PTD, PTD, PTD,
                                  PTD, PTA, PTA,
                                  PTD, PTD, PTC
                                };

					
fingers thumb_f =   {WAITC,0,0,160};
fingers index_f =   {WAITC,0,0,160};
fingers middle_f =  {WAITC,0,0,160};
fingers ring_f =    {WAITC,0,0,160};
fingers little_f =  {WAITC,0,0,120};
fingers thumb_rot = {WAITC,0,0,160};

uint8_t btn = 0;																	// Activate / deactivate 
uint8_t cmd = 0;																	// LCD commands
uint32_t ticks = 0;																// 1 ms ticks
uint8_t i = 0;

void Finger_Close(uint8_t finger_m);
void Finger_Open(uint8_t finger_m);
void Finger_Stop(uint8_t finger_m);
void Finger_Timing(uint8_t *state, uint32_t * time_ms);
void Finger_Action(fingers * finger_f, uint8_t finger, uint8_t action);

int main(void){
	Switch_Init();
	LED_Init(); 
	ADC0_Init();
	UART0_Init();
	Output_Init();
	SysTick_Config(SystemCoreClock/1000);
	
	arm_fill_q15(0, little_f.buffer, SIZE);
	
	while(1){
		
		if(btn) Finger_Action(&little_f, 1, CLOSE);
		else Finger_Action(&little_f, 1, OPEN);
		
		//Finger_Close(1);
		//Finger_Open(1);
		
	}
}

void SysTick_Handler(void) {
	//char String[16];
	little_f.buffer[ticks%SIZE] = (int16_t) ADC0_Read(3);
	ring_f.buffer[ticks%SIZE] = (int16_t) ADC0_Read(4);
	middle_f.buffer[ticks%SIZE] = (int16_t) ADC0_Read(5);
	index_f.buffer[ticks%SIZE] = (int16_t) ADC0_Read(6);
	
	Finger_Timing(&little_f.state, &little_f.time_ms);
	
	//itoa(little_f.time_ms,String);
	//itoa(little_f.mean,String);
	//UART0_putString(String);
	ticks++; 
}

void PORTC_IRQHandler(void){
	if(PORTC->PCR[6]&PORT_PCR_ISF_MASK){
		if(cmd<5) cmd++;
		else cmd = 0;
		UART0_send(cmd+'0');
		PORTC->PCR[6] |= (PORT_PCR_ISF_MASK);
	}
	if(PORTC->PCR[7]&PORT_PCR_ISF_MASK){
		if(btn<1) {
			btn++;
			LED_On();
		}
		else {
			btn = 0;
			LED_Off();
		}
		PORTC->PCR[7] |= (PORT_PCR_ISF_MASK);
	}
}
	
void UART0_RX_TX_IRQHandler(void){
	uint8_t data ;
  (void) UART0->S1;
	data = UART0->D;
	UART0->D = data;
}

void Finger_Timing(uint8_t *state, uint32_t *time_ms){
	switch(*state){
		case OPEN:  *time_ms -= 1; break;
		case WAITC: *time_ms = 0; break;
		case CLOSE: *time_ms += 1; break;
		case WAITO: *time_ms += 0;
	}
}

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

void Finger_Action(fingers * finger_f, uint8_t finger_m, uint8_t action){
	if(action == CLOSE){
		switch(finger_f->state){
			case OPEN:{
				finger_f->state = CLOSE;
				Finger_Stop(finger_m);
			} break;
			case WAITC:{
				finger_f->state = CLOSE;
				Finger_Stop(finger_m);
			} break;
			case CLOSE:{
				arm_mean_q15(finger_f->buffer,SIZE,&finger_f->mean);
				if((little_f.mean<finger_f->threshold)&&(finger_f->state == CLOSE)){
					finger_f->state = CLOSE;
					Finger_Close(finger_m);
				}
				else { 
					finger_f->state = WAITO;
					Finger_Stop(finger_m);
				}	
			} break;
			case WAITO:{
				finger_f->state = WAITO;
				Finger_Stop(finger_m);
			} break;
		}
	} else{	//OPEN
		switch(finger_f->state){
			case OPEN:{
				if((finger_f->time_ms>0)&&(finger_f->state == OPEN)){
					finger_f->state = OPEN;
					Finger_Open(finger_m);
				}
				else{
					finger_f->state = WAITC;
					finger_f->time_ms = 0;
					Finger_Stop(finger_m);
				}
			} break;
			case WAITC:{
				finger_f->state = WAITC;
				Finger_Stop(finger_m);
			} break;
			case CLOSE:{
				finger_f->state = OPEN;
				Finger_Stop(finger_m);
			} break;
			case WAITO:{
				finger_f->state = OPEN;
				Finger_Stop(finger_m);
			} break;
		}
	}
}
