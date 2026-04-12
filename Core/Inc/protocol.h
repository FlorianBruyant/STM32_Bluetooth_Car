/*
 * @file protocol.h
 * @brief contains command parser and rx process functions for UART
 */


#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "stm32f411xe.h"
#include "motor_control.h"
#include "uart.h"
#include "timer.h"
#include <stdint.h>

typedef enum
{
    STATE_WAIT_DIR = 0U,  /* Waiting for direction byte    */
    STATE_WAIT_D1,        /* Waiting for first speed digit */
    STATE_WAIT_D2         /* Waiting for second speed digit */
} ParserState_t;

/**
 * @brief  Process all bytes currently available in the UART RX buffer.
 *
 *         Must be called repeatedly from the main loop.
 *         When a complete, valid frame is received the corresponding motor
 *         command is executed, an "OK\r\n" acknowledge is sent over UART,
 *         and *last_cmd_tick is updated with the current SysTick value so
 *         the caller can reset its watchdog.
 *
 * @param  last_cmd_tick  Pointer to the watchdog timestamp (milliseconds).
 *                        Updated on every successfully decoded frame.
 */
void protocol_ProcessRx(uint32_t *last_cmd_tick);

#endif /* PROTOCOL_H */
