/*
 * gpio.h
 *
 *  Created on: 10 avr. 2026
 *      Author: Florian Bruyant
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include "CMSIS/stm32f411xe.h"

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
		uint8_t pin,
		uint8_t mode,
		uint8_t outputType,
		uint8_t outputSpeed,
		uint8_t pullUpDown);

#endif /* DRIVERS_GPIO_H_ */
