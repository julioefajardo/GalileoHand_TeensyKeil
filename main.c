/* Universidad Galileo
 * Turing Research Lab
 * Julio E. Fajardo
 * Galileo Bionic Hand
 * CMSIS-DSP Application
 * Prostheses Controller
 * main.c
 */

#define ARM_MATH_CM4

#include "MK20D7.h"                     // Device header
#include "arm_math.h"                   // ARM::CMSIS:DSP
#include <stdlib.h>
#include "drivers.h"
															
fingers thummb_f =  {WAITC,0,0,160};
fingers index_f =   {WAITC,0,0,160};
fingers middle_f =  {WAITC,0,0,160};
fingers ring_f =    {WAITC,0,0,160};
fingers little_f =  {WAITC,0,0,160};
fingers thumb_rot = {WAITC,0,0,160};

uint8_t btn = 0;												// Activate / deactivate 
uint8_t cmd = 0;												// LCD commands
uint32_t ticks = 0;											// 1 ms ticks

int main(void){
	Switch_Init();
	LED_Init(); 
	ADC0_Init();
	UART0_Init();
	Output_Init();
	SysTick_Config(SystemCoreClock/10);
	
	while(1){
		Finger_Close(1);
		Finger_Close(2);
	}
	
}

void SysTick_Handler(void) {
	LED_Toggle();
	ticks++; 
}

void PORTC_IRQHandler(void){
	if(PORTC->PCR[6]&PORT_PCR_ISF_MASK){
		if(cmd<4) cmd++;
		else cmd = 0;
		PORTC->PCR[6] |= (PORT_PCR_ISF_MASK);
	}
	if(PORTC->PCR[7]&PORT_PCR_ISF_MASK){
		if(btn<2) btn++;
		else btn = 0;
		PORTC->PCR[7] |= (PORT_PCR_ISF_MASK);
	}
}
	
void UART0_RX_TX_IRQHandler(void){
	uint8_t data ;
  (void) UART0->S1;
	data = UART0->D;
	UART0->D = data;
}
