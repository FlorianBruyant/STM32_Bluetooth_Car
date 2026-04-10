/*
 * gpio.h
 *
 *  Created on: 10 avr. 2026
 *      Author: Florian Bruyant
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include "CMSIS/stm32f411xe.h"

void GPIO_Init(GPIO_TypeDef* port,
		uint8_t pin,
		uint8_t mode,
		uint8_t outputType,
		uint8_t outputSpeed,
		uint8_t pullUpDown);

#endif /* DRIVERS_GPIO_H_ */
