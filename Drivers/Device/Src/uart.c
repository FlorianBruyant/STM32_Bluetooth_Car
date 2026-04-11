/*
 * uart.c
 *
 *  Created on: 11 avr. 2026
 *      Author: Florian Bruyant
 */

#include "uart.h"

void UART_Init(){
	// Init clocks for GPIOA and USART1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// Reset USART1
	RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

	// Configure PA9 to USART1_TX (AF7)
	GPIO_Init(GPIOA, 9U, GPIO_MODE_ALTERNATE, GPIO_OTYPE_PUSHPULL, GPIO_SPEED_FAST, GPIO_PULL_NONE);
	GPIO_SelectAltFunc(GPIOA, 9U, GPIO_AF_USART1);

	// Configure PA10 to USART1_RX (AF7)
	GPIO_Init(GPIOA, 10U, GPIO_MODE_ALTERNATE, GPIO_OTYPE_PUSHPULL, GPIO_SPEED_FAST, GPIO_PULL_NONE);
	GPIO_SelectAltFunc(GPIOA, 10U, GPIO_AF_USART1);

	USART1->BRR = 1667U; // Value that sets baud rate to 9600 at input frequency of 16 MHz

	USART1->CR1 |= USART_CR1_TE      // Transmitter enable
				|  USART_CR1_RE      // Receiver enable
				|  USART_CR1_UE;
}
