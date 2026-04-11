/*
 * uart.c
 *
 *  Created on: 11 avr. 2026
 *      Author: Florian Bruyant
 */

#include "uart.h"

// RX Ring buffer
static volatile uint8_t  rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint32_t rx_head = 0U;
static volatile uint32_t rx_tail = 0U;

// TX Ring buffer
static volatile uint8_t  tx_buffer[UART_TX_BUFFER_SIZE];
static volatile uint32_t tx_head = 0U;
static volatile uint32_t tx_tail = 0U;

void UART_Init(void){
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
				|  USART_CR1_RXNEIE  // IRQ when RX not empty
				|  USART_CR1_UE;

	// Activate USART1 interrupt
	NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void){
	uint32_t sr = USART1->SR;
	// Check that the interrupt comes from RXNE
	if(sr & USART_SR_RXNE){
		// Only the first 8 bits are used, the rest are reserved
		uint8_t data = (uint8_t)(USART1->DR & 0xFFU);

		uint32_t next_head = (rx_head + 1U) % UART_RX_BUFFER_SIZE;

		// Check if the buffer is not empty
		if (next_head != rx_tail)
		{
			rx_buffer[rx_head] = data;
			rx_head = next_head;
		}
		// TODO : increment error counter if buffer is full
	}
	if((sr & USART_SR_TXE) && (USART1->CR1 & USART_CR1_TXEIE)){
		if(tx_tail != tx_head){
			USART1->DR = (uint32_t)tx_buffer[tx_tail];
			tx_tail = (tx_tail + 1U) % UART_TX_BUFFER_SIZE;
		}
		else{
			// Disable interrupt when tx buffer is empty
			USART1->CR1 &= ~USART_CR1_TXEIE;
		}
	}
}

uint8_t UART_Read(uint8_t* data){
	// Check if new data is available
	if(rx_head==rx_tail){
		return 0U;
	}
	*data = rx_buffer[rx_tail];
	// Move the tail to the next position
	rx_tail = (rx_tail + 1U) % UART_RX_BUFFER_SIZE;
	return 1U;
}

uint8_t UART_SendByte(uint8_t byte)
{
    uint32_t next_head = (tx_head + 1U) % UART_TX_BUFFER_SIZE;

    // Check if buffer is full
    if (next_head == tx_tail)
    {
        return 0U;
    }

    tx_buffer[tx_head] = byte;
    tx_head = next_head;

    USART1->CR1 |= USART_CR1_TXEIE;

    return 1U;
}

void UART_SendString(const char *str)
{
    if (str == NULL) return;
    while (*str != '\0')
    {
        while (!UART_SendByte((uint8_t)*str)) {}
        str++;
    }
}
