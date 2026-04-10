/**
 * @file      motor_control.c
 * @brief     Implementation of DC motors controls
 */

#include "../Inc/motor_control.h"

#include <stdint.h>

/* Base addresses */

// Base address of the clock control block
#define RCC_BASE 0x40023800U

// Clock register to turn GPIO on or off
#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x30U))

// Base addresses for GPIO A and B ports
#define GPIOA_BASE 0x40020000U
#define GPIOB_BASE 0x40020400U

// GPIOA MODER and BSSR
#define GPIOA_MODER   (*(volatile uint32_t *)(GPIOA_BASE + 0x00U))
#define GPIOA_BSRR    (*(volatile uint32_t *)(GPIOA_BASE + 0x18U))

// GPIOB MODER and BSSR
#define GPIOB_MODER   (*(volatile uint32_t *)(GPIOB_BASE + 0x00U))
#define GPIOB_BSRR    (*(volatile uint32_t *)(GPIOB_BASE + 0x18U))
