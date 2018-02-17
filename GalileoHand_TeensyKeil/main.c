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
#include <stdio.h>
#include "drivers.h"
#include "finger.h"

#define DEACTIVATED 0
#define ACTIVATED   1

int16_t value = 0;
char debug[32];

int16_t encoding = 0;

typedef struct electrode{
  q15_t mean;
  q15_t threshold;
  q15_t buffer[ESIZE];
  q15_t rectified[ESIZE];
  } electrodes;

electrodes E1 = {0,500};
electrodes E2 = {0,500};

uint8_t muscle_state = DEACTIVATED;

fingers thumb_rot = {WAITC,6,0,400,0,80};
fingers thumb_f =   {WAITC,5,0,200,0,80};
fingers index_f =   {WAITC,4,0,200,0,120};
fingers middle_f =  {WAITC,3,0,200,0,100};
fingers ring_f =    {WAITC,2,0,200,0,80};
fingers little_f =  {WAITC,1,0,200,0,80};

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

void Hand_Action(uint8_t hand_action);
void pause(void);															
															
int main(void){
  Switch_Config();
  LED_Config(); 
  ADC0_Config();
  PIT_Init(1000);
  UART0_Config();
  Output_Config();
  SysTick_Config(SystemCoreClock/1000);
  QD_Init(&encoding);
	
  arm_fill_q15(0, little_f.buffer, SIZE);
  arm_fill_q15(0, ring_f.buffer, SIZE);
  arm_fill_q15(0, middle_f.buffer, SIZE);
  arm_fill_q15(0, index_f.buffer, SIZE);
  arm_fill_q15(0, thumb_f.buffer, SIZE);
	
  arm_fill_q15(0, E1.buffer, ESIZE);
  arm_fill_q15(0, E2.buffer, ESIZE);
	
  pause();
		
  while(1){
    arm_abs_q15(E1.buffer,E1.rectified,ESIZE);
    arm_abs_q15(E2.buffer,E2.rectified,ESIZE);
    arm_mean_q15(E1.rectified,ESIZE,&E1.mean);
    arm_mean_q15(E2.rectified,ESIZE,&E2.mean);
		
    switch(muscle_state){
      case DEACTIVATED:{
        if(E1.mean>E1.threshold){
          LED_On();
          muscle_state = ACTIVATED;
			  }
      } break;
      case ACTIVATED:{
        if(E2.mean>E2.threshold){
          LED_Off();
          muscle_state = DEACTIVATED;
        }
      } 
    }
		
    if(muscle_state){
      switch(cmd){
        case POWER:   Hand_Action(POWER);   break;
        case POINT:   Hand_Action(POINT);   break;		
        case PINCH:   Hand_Action(PINCH);   break;
        case HOOK:    Hand_Action(HOOK);    break;
        case LATERAL: Hand_Action(LATERAL); break;
        case PEACE:   Hand_Action(PEACE);   break;
        default:      Hand_Action(POWER); 
      }
    }
    else Hand_Action(REST);
		
    /*if(btn){
      switch(cmd){
        case POWER:   Hand_Action(POWER);   break;
        case POINT:   Hand_Action(POINT);   break;		
        case PINCH:   Hand_Action(PINCH);   break;
        case HOOK:    Hand_Action(HOOK);    break;
        case LATERAL: Hand_Action(LATERAL); break;
        case PEACE:   Hand_Action(PEACE);   break;
        default:      Hand_Action(POWER); 
      }
    }
    else Hand_Action(REST);*/
  }
}

void SysTick_Handler(void) {
  little_f.buffer[ticks%SIZE] = (int16_t) ADC0_Read(2);
  ring_f.buffer[ticks%SIZE]   = (int16_t) ADC0_Read(3);
  middle_f.buffer[ticks%SIZE] = (int16_t) ADC0_Read(4);
  index_f.buffer[ticks%SIZE]  = (int16_t) ADC0_Read(5);
  thumb_f.buffer[ticks%SIZE]  = (int16_t) ADC0_Read(6);
	
  Finger_Timing(&little_f);
  Finger_Timing(&ring_f);
  Finger_Timing(&middle_f);
  Finger_Timing(&index_f);
  Finger_Timing(&thumb_f);
  Finger_Timing(&thumb_rot);
	
  ticks++; 
}

void PIT0_IRQHandler(void){
  PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;																										
  E1.buffer[ticks%(ESIZE)] = ADC0_Read(0)-1396;
  E2.buffer[ticks%(ESIZE)] = ADC0_Read(1)-1396;
  QD_Process(&encoding);
}

void PORTC_IRQHandler(void){
  if(PORTC->PCR[6]&PORT_PCR_ISF_MASK){
    if(cmd<5) cmd++;
    else cmd = 0;
    UART0_send(cmd+'0');
    PORTC->PCR[6] |= (PORT_PCR_ISF_MASK);
  }
  if(PORTC->PCR[7]&PORT_PCR_ISF_MASK){
    //if(btn<1) btn++;
    //else btn = 0;
    if(cmd>0) cmd--;
    else cmd = 5;
    UART0_send(cmd+'0');
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
  Finger_Action(&little_f, actions[hand_action][0],0);
  Finger_Action(&ring_f, actions[hand_action][1],0);
  Finger_Action(&middle_f, actions[hand_action][2],0);
  Finger_Action(&index_f, actions[hand_action][3],0);
  Finger_Action(&thumb_f, actions[hand_action][4],0);
  Finger_ActionTime(&thumb_rot, actions[hand_action][5],0);
}

void pause(void){
  for(volatile int i=0;i<5000;i++){	
    for(volatile int j=0;j<2000;j++);
  }
}
