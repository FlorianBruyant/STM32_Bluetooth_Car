#ifndef UART_H_
#define UART_H_

#include "../CMSIS/stm32f411xe.h"
#include "gpio.h"

#define UART_RX_BUFFER_SIZE 64U
#define UART_TX_BUFFER_SIZE 64U

/**
 * @brief  Initializes USART1 at 9600 baud (assuming 16MHz APB2 clock).
 * @note   Configures PA9 (TX) and PA10 (RX) in Alternate Function 7.
 * Enables RXNE (Receive Not Empty) interrupt.
 */
void UART_Init(void);

/**
 * @brief  Reads a single byte from the RX ring buffer.
 * @param  data: Pointer to the variable where the read byte will be stored.
 * @return 1U if a byte was successfully read, 0U if the buffer is empty.
 */
uint8_t UART_Read(uint8_t* data);

/**
 * @brief  Puts a single byte into the TX ring buffer and starts transmission.
 * @note   Automatically enables the TXE (Transmit Data Register Empty) interrupt.
 * @param  byte: The byte to be transmitted.
 * @return 1U if the byte was successfully added to the buffer, 0U if the buffer is full.
 */
uint8_t UART_SendByte(uint8_t byte);

/**
 * @brief  Sends a null-terminated string via UART.
 * @note   This function blocks if the TX ring buffer becomes full during operation,
 * waiting for space to free up before continuing.
 * @param  str: Pointer to the null-terminated string to send.
 */
void UART_SendString(const char *str);

#endif
