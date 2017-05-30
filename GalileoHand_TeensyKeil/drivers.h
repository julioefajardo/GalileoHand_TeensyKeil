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

void LED_Config(void);
void Output_Config(void);
void Switch_Config(void);
void ADC0_Config(void);
void FTM0_Config(void);
void UART0_Config(void);
void UART2_Config(void);

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
