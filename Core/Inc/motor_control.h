/**
 * @file      motor_control.h
 * @brief     Header file for motor_control.c
 */

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#include <stdint.h>
#include "../../Drivers/Device/Inc/gpio.h"

/**
 * TODO: Right now the PWM frequency is set to 1 kHz
 * (f_PWM = 16 000 000 / ((15+1) × (999+1)) = 16 000 000 / 16 000 = 1 kHz)
 * Test different values to see which gives the best results
 */
#define PWM_ARR   999U
#define PWM_PSC   15U

/* Direction pins (GPIO output) */
#define IN1_PIN   0U    /* PA0 - Left motors,  direction A */
#define IN2_PIN   1U    /* PA1 - Left motors,  direction B */
#define IN3_PIN   4U    /* PA4 - Right motors, direction A */
#define IN4_PIN   0U    /* PB0 - Right motors, direction B */

/* PWM pins (TIM3 alternate function AF2) */
#define ENA_PIN   4U    /* PB4 - TIM3_CH1 - Left motors speed */
#define ENB_PIN   5U    /* PB5 - TIM3_CH2 - Right motors speed */

/**
 * @brief  Initialize GPIO direction pins (IN1–IN4) and
 *         TIM3 PWM outputs (ENA on PB4, ENB on PB5).
 *         Motors start stopped at 0% duty cycle.
 */
void motor_Init(void);

/**
 * @brief  Set left motor speed.
 * @param  speed  Duty cycle percentage, clamped to [0, 100].
 */
void motor_SetSpeedLeft(uint8_t speed);

/**
 * @brief  Set right motor speed.
 * @param  speed  Duty cycle percentage, clamped to [0, 100].
 */
void motor_SetSpeedRight(uint8_t speed);

/**
 * @brief  Set both motors to the same speed.
 * @param  speed  Duty cycle percentage, clamped to [0, 100].
 */
void motor_SetSpeed(uint8_t speed);

/**
 * @brief  Drive both motors forward at given speed.
 * @param  speed  Duty cycle percentage, clamped to [0, 100].
 */
void motor_Forward(uint8_t speed);

/**
 * @brief  Drive both motors backward at given speed.
 * @param  speed  Duty cycle percentage, clamped to [0, 100].
 */
void motor_Backward(uint8_t speed);

/**
 * @brief  Spin left : left motor backward, right motor forward.
 * @param  speed  Duty cycle percentage, clamped to [0, 100].
 */
void motor_TurnLeft(uint8_t speed);

/**
 * @brief  Spin right : left motor forward, right motor backward.
 * @param  speed  Duty cycle percentage, clamped to [0, 100].
 */
void motor_TurnRight(uint8_t speed);

/**
 * @brief  Stop both motors (free wheel).
 *         PWM duty cycle is set to 0 before releasing direction pins.
 */
void motor_Stop(void);

#endif /* MOTOR_CONTROL_H_ */
