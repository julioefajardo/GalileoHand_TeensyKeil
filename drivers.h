/* Universidad Galileo
 * Turing Research Lab
 * Julio E. Fajardo
 * Galileo Bionic Hand
 * CMSIS-DSP Application
 * Embedded Prostheses Controller
 * May-09-2017
 * drivers.h
 */
 
#ifndef DRIVERS_H_
#define DRIVERS_H_

#include <stdint.h>
#include <stdlib.h>
#include "arm_math.h"                   // ARM::CMSIS:DSP

#define 		SIZE 			50

#define			OPEN			0
#define			WAITC			1
#define			CLOSE			2
#define			WAITO			3

#define			POWER			0
#define			POINT			1
#define			PINCH			2
#define			HOOK			3
#define			LATERAL		4
#define			PEACE			5

typedef struct finger{
	uint8_t state;
  uint32_t time_ms;	
	q15_t mean;
	q15_t threshold;
	q15_t buffer[SIZE];
	} fingers;

void LED_Init(void);
void Output_Init(void);
void Switch_Init(void);
void ADC0_Init(void);
void FTM0_Init(void);
void UART0_Init(void);
void UART2_Init(void);

void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);

uint16_t ADC0_Read(unsigned int channel);
void UART0_send(char dato);
void UART0_putString(char *mystring);
void UART2_send(char dato);
void UART2_putString(char *mystring);

void reverse(char s[]);
void itoa(int32_t n, char s[]);
#endif /* DRIVERS_H_ */
