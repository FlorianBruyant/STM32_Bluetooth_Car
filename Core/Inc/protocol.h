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
#include "vl53l0x_api.h"
#include <stdint.h>

typedef enum
{
    STATE_WAIT_DIR = 0U,  /* Waiting for direction byte    */
    STATE_WAIT_D1,        /* Waiting for first speed digit */
    STATE_WAIT_D2         /* Waiting for second speed digit */
} ParserState_t;

// Auto brake distance configuration (in mm)
#define BRAKE_DISTANCE_MM 200U

/*
 * @brief Used to parse UART commands and execute the corresponding action
 * 		  FORMAT : 1 int dir, 2 int motor power in percentage,
 * 		  Ex : F70 ; Forward with 70% power
 *
 * @param dir Direction  ASCII character for direction
 * 			  Possible directions : F,B,L,R,S
 * @param d1  First digit for power
 * @param d2  Second digit for power
 *
 * @return 0 if direction is invalid or there is an obstacle in front, 1 else
 */
uint8_t parse_and_execute(uint8_t dir, uint8_t d1, uint8_t d2);

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

/**
 * @brief Complete initialization and test protocol for VL53L0X ToF sensor
 */
void tof_init_protocol(void);

/*
 * @brief Checks if the sensor detects an obstacle : Stops the motors if there is one
 */
void check_obstacle(void);

#endif /* PROTOCOL_H */
