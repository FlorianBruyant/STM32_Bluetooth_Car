/*
 * gpio.h
 *
 *  Created on: 10 avr. 2026
 *      Author: Florian Bruyant
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include "../CMSIS/stm32f411xe.h"

/**
 * @brief  Initializes the GPIOx peripheral according to the specified parameters.
 * @param  port:       Pointer to GPIOx peripheral base address (e.g., GPIOA, GPIOB).
 * @param  pin:        Specifies the port pin to be configured (0 to 15).
 * @param  mode:       Operating mode for the selected pin (Input, Output, Alternate, Analog).
 * @param  outputType: Operating output type for the selected pin (Push-pull or Open-drain).
 * @param  outputSpeed: Specifies the speed for the selected pin (Low, Medium, Fast, High).
 * @param  pullUpDown: Specifies the Pull-up or Pull-down activation for the selected pin.
 */
void GPIO_Init(GPIO_TypeDef* port,
		uint32_t pin,
		uint32_t mode,
		uint32_t outputType,
		uint32_t outputSpeed,
		uint32_t pullUpDown);

/**
 * @brief  Sets or clears the selected data port pin, using the BSSR.
 * @param  port:  Pointer to GPIOx peripheral base address (e.g., GPIOA, GPIOB).
 * @param  pin:   Specifies the port pin to be written (0 to 15).
 * @param  value: Specifies the value to be written to the selected pin.
 * This parameter can be one of the following:
 * @arg 0: to clear the pin (Logic Low)
 * @arg 1: to set the pin (Logic High)
 */
void GPIO_WritePin(GPIO_TypeDef* port,
		uint32_t pin,
		uint32_t value);

#endif /* DRIVERS_GPIO_H_ */
