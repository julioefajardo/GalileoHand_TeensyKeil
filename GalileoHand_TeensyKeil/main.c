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
#include "finger.h"

fingers thumb_rot = {WAITC,6,0,200,0,120};
fingers thumb_f =   {WAITC,5,0,200,0,120};
fingers index_f =   {WAITC,4,0,200,0,120};
fingers middle_f =  {WAITC,3,0,200,0,150};
fingers ring_f =    {WAITC,2,0,200,0,120};
fingers little_f =  {WAITC,1,0,200,0,120};

const uint8_t actions[7][6] = { CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE,   // Power Grip 
                                CLOSE, CLOSE, CLOSE,  OPEN, CLOSE, CLOSE,   // Point
                                OPEN,  OPEN,  OPEN,  CLOSE, CLOSE, CLOSE,   // Pinch
                                CLOSE, CLOSE, CLOSE, CLOSE, OPEN,  OPEN,    // Hook
                                CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, OPEN,    // Lateral
                                CLOSE, CLOSE, OPEN,   OPEN, CLOSE, CLOSE,   // Peace
                                OPEN,  OPEN,  OPEN,   OPEN, OPEN,  OPEN	    // Open
                              }; 


uint8_t btn = 0;                                                            // Activate / deactivate 
uint8_t cmd = 0;                                                            // LCD commands
uint32_t ticks = 0;                                                         // 1 ms ticks
uint8_t i = 0;

int main(void){
	Switch_Config();
	LED_Config(); 
	ADC0_Config();
	UART0_Config();
	Output_Config();
	SysTick_Config(SystemCoreClock/1000);
	
	arm_fill_q15(0, little_f.buffer, SIZE);
	arm_fill_q15(0, ring_f.buffer, SIZE);
	arm_fill_q15(0, middle_f.buffer, SIZE);
	arm_fill_q15(0, index_f.buffer, SIZE);
	
	while(1){
		
		if(btn){ 
			switch(cmd){
				case POWER:   Hand_Action(POWER);   break;
				case POINT:   Hand_Action(POINT);   break;		
				case PINCH:   Hand_Action(PINCH);   break;
				case HOOK:    Hand_Action(HOOK);    break;
				case LATERAL: Hand_Action(LATERAL); break;
				case PEACE:   Hand_Action(PEACE);   break;
				default:      Hand_Action(POWER); 
			}
			//Finger_Action(&little_f, CLOSE);
			//Finger_Action(&ring_f, CLOSE);
			//Finger_Action(&middle_f, CLOSE);
			//Finger_Action(&index_f, CLOSE);
			//Finger_Rotation(&thumb_rot, CLOSE);
		}
		else {
			Hand_Action(REST);
			//Finger_Action(&little_f, OPEN);
			//Finger_Action(&ring_f, OPEN);
			//Finger_Action(&middle_f, OPEN);
			//Finger_Action(&index_f, OPEN);
			//Finger_Rotation(&thumb_rot, OPEN);
		}
		
		//Finger_Close(1);
		//Finger_Open(1);	
	}
}

void SysTick_Handler(void) {
	little_f.buffer[ticks%SIZE] = (int16_t) ADC0_Read(3);
	ring_f.buffer[ticks%SIZE]   = (int16_t) ADC0_Read(4);
	middle_f.buffer[ticks%SIZE] = (int16_t) ADC0_Read(5);
	index_f.buffer[ticks%SIZE]  = (int16_t) ADC0_Read(6);
	
	Finger_Timing(&little_f);
	Finger_Timing(&ring_f);
	Finger_Timing(&middle_f);
	Finger_Timing(&index_f);
	Finger_Timing(&thumb_rot);
	
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

void Hand_Action(uint8_t hand_action){
	Finger_Action(&little_f, actions[hand_action][0]);
	Finger_Action(&ring_f, actions[hand_action][1]);
	Finger_Action(&middle_f, actions[hand_action][2]);
	Finger_Action(&index_f, actions[hand_action][3]);
	Finger_Action(&thumb_f, actions[hand_action][4]);
	Finger_Rotation(&thumb_rot, actions[hand_action][5]);
}
