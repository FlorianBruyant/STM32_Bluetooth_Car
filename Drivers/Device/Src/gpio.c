/*
 * gpio.c
 *
 *  Created on: 10 avr. 2026
 *      Author: Florian Bruyant
 */

#include "gpio.h"

void GPIO_Init(GPIO_TypeDef* port,
		uint32_t pin,
		uint32_t mode,
		uint32_t outputType,
		uint32_t outputSpeed,
		uint32_t pullUpDown){

	/* GPIO MODE CONFIGURATION */

	// Clear the 2 bits corresponding to the mode of the pin
	port->MODER &= ~(0x3UL << (pin*2U));

	// Set mode for selected pin
	port->MODER |= (mode << (pin*2U));

	/* OUTPUT TYPE CONFIGURATION */
	// 1 bit used in OTYPER

	port->OTYPER &= ~(0x1UL << pin);

	port->MODER |= (outputType << pin);

	/* OUTPUT SPEED CONFIGURATION */
	// 2 bits used in OSPEEDR

	port->OSPEEDR &= ~(0x3UL << (pin*2U));

	port->OSPEEDR |= (outputSpeed << (pin*2U));

	/* OUTPUT PULL UP/DOWN CONFIGURATION */
	// 2 bits used

	port->PUPDR &= ~(0x3UL << (pin*2U));

	port->PUPDR |= (pullUpDown << (pin*2U));
}

void GPIO_WritePin(GPIO_TypeDef* port, uint32_t pin, uint32_t value){
	// Sets the bit (1U) or resets
	if(value == 1U){
		port->BSRR |= (1UL << pin);
	} else{
		port->BSRR |= (1UL << (pin+16U));
	}
}

GPIO_PinState_t GPIO_ReadPin(GPIO_TypeDef* port, uint32_t pin){
	if((port->IDR & (1UL << pin)) != 0U){
		return GPIO_PIN_SET;
	}else{
		return GPIO_PIN_RESET;
	}
}
