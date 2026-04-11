/*
 * gpio.h
 *
 *  Created on: 10 avr. 2026
 *      Author: Florian Bruyant
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "../CMSIS/stm32f411xe.h"

/* Alternate function mapping */
#define GPIO_AF_USART1 7U

/**
 * @brief GPIO Pin Mode (MODER register)
 */
typedef enum {
    GPIO_MODE_INPUT     = 0x00U,
    GPIO_MODE_OUTPUT    = 0x01U,
    GPIO_MODE_ALTERNATE = 0x02U,
    GPIO_MODE_ANALOG    = 0x03U
} GPIO_Mode_t;

/**
 * @brief GPIO Output Type (OTYPER register)
 */
typedef enum {
    GPIO_OTYPE_PUSHPULL  = 0x00U,
    GPIO_OTYPE_OPENDRAIN = 0x01U
} GPIO_OType_t;

/**
 * @brief GPIO Output Speed (OSPEEDR register)
 */
typedef enum {
    GPIO_SPEED_LOW       = 0x00U,
    GPIO_SPEED_MEDIUM    = 0x01U,
    GPIO_SPEED_FAST      = 0x02U,
    GPIO_SPEED_HIGH      = 0x03U
} GPIO_Speed_t;

/**
 * @brief GPIO Pull-up/Pull-down (PUPDR register)
 */
typedef enum {
    GPIO_PULL_NONE       = 0x00U,
    GPIO_PULL_UP         = 0x01U,
    GPIO_PULL_DOWN       = 0x02U
} GPIO_Pull_t;

/**
 * @brief GPIO Pin State
 */
typedef enum {
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET   = 1U
} GPIO_PinState_t;

/**
 * @brief  Initializes the GPIOx peripheral according to the specified parameters.
 * @param  port:        Pointer to GPIOx peripheral base address (e.g., GPIOA, GPIOB).
 * @param  pin:         Specifies the port pin to be configured (0 to 15).
 * @param  mode:        Operating mode for the selected pin.
 * This parameter must be a value of @ref GPIO_Mode_t.
 * @param  outputType:  Operating output type for the selected pin.
 * This parameter must be a value of @ref GPIO_OType_t.
 * @param  outputSpeed: Specifies the speed for the selected pin.
 * This parameter must be a value of @ref GPIO_Speed_t.
 * @param  pullUpDown:  Specifies the Pull-up or Pull-down activation for the selected pin.
 * This parameter must be a value of @ref GPIO_Pull_t.
 * @retval None
 */
void GPIO_Init(GPIO_TypeDef* port,
		uint8_t     pin,
		GPIO_Mode_t  mode,
		GPIO_OType_t outputType,
		GPIO_Speed_t outputSpeed,
		GPIO_Pull_t  pullUpDown);

/**
 * @brief  Sets or clears the selected data port pin using the BSRR register.
 * @param  port:  Pointer to GPIOx peripheral base address (e.g., GPIOA, GPIOB).
 * @param  pin:   Specifies the port pin to be written (0 to 15).
 * @param  value: Specifies the value to be written to the selected pin.
 * This parameter must be a value of @ref GPIO_PinState_t.
 * @retval None
 */
void GPIO_WritePin(GPIO_TypeDef* port,
		uint8_t        pin,
		GPIO_PinState_t value);

/**
 * @brief  Reads the specified input port pin.
 * @param  port: Pointer to GPIOx peripheral base address (e.g., GPIOA, GPIOB).
 * @param  pin:  Specifies the port pin to read (0 to 15).
 * @retval The input port pin value.
 * This return value is a member of @ref GPIO_PinState_t.
 */
GPIO_PinState_t GPIO_ReadPin(GPIO_TypeDef* port, uint8_t pin);

/**
 * @brief  Configures the alternate function (AF) for a specific GPIO pin.
 * @param  port:     Pointer to GPIOx peripheral base address (e.g., GPIOA, GPIOB).
 * @param  pin:      Specifies the port pin to be configured (0 to 15).
 * @param  alt_func: Specifies the alternate function selection (0 to 15).
 * @retval None
 */
void GPIO_SelectAltFunc(GPIO_TypeDef* port, uint8_t pin, uint8_t alt_func);

#endif /* GPIO_H_ */
