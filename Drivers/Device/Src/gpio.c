/*
 * gpio.c
 *
 *  Created on: 10 avr. 2026
 *      Author: Florian Bruyant
 */

#include "gpio.h"

void GPIO_Init(GPIO_TypeDef* port,
		uint8_t pin,
		GPIO_Mode_t mode,
		GPIO_OType_t outputType,
		GPIO_Speed_t outputSpeed,
		GPIO_Pull_t pullUpDown){

	/* GPIO MODE CONFIGURATION */

	// Clear the 2 bits corresponding to the mode of the pin
	port->MODER &= ~(0x3UL << (pin*2U));

	// Set mode for selected pin
	port->MODER |= ((uint32_t)mode << (pin*2U));

	/* OUTPUT TYPE CONFIGURATION */
	// 1 bit used in OTYPER

	port->OTYPER &= ~(0x1UL << pin);

	port->OTYPER |= ((uint32_t)outputType << pin);

	/* OUTPUT SPEED CONFIGURATION */
	// 2 bits used in OSPEEDR

	port->OSPEEDR &= ~(0x3UL << (pin*2U));

	port->OSPEEDR |= ((uint32_t)outputSpeed << (pin*2U));

	/* OUTPUT PULL UP/DOWN CONFIGURATION */
	// 2 bits used

	port->PUPDR &= ~(0x3UL << (pin*2U));

	port->PUPDR |= ((uint32_t)pullUpDown << (pin*2U));
}

void GPIO_WritePin(GPIO_TypeDef* port, uint8_t pin, GPIO_PinState_t value){
	// Sets the bit (1U) or resets
	if(value == GPIO_PIN_SET){
		port->BSRR = (1UL << pin);
	} else{
		port->BSRR = (1UL << (pin+16U));
	}
}

GPIO_PinState_t GPIO_ReadPin(GPIO_TypeDef* port, uint8_t pin){
	if((port->IDR & (1UL << pin)) != 0U){
		return GPIO_PIN_SET;
	}else{
		return GPIO_PIN_RESET;
	}
}

void GPIO_SelectAltFunc(GPIO_TypeDef* port, uint8_t pin, uint8_t alt_func){
	// Mask to disable modification of the upper 4 bits
	alt_func &= 0xFU;

	if(pin <= 7){
		port->AFR[0] &= ~(0xFUL << (pin*4U));
		port->AFR[0] |= ((uint32_t)alt_func << (pin*4U));
	}else{
		port->AFR[1] &= ~(0xFUL << ((pin-8U)*4U));
		port->AFR[1] |= ((uint32_t)alt_func << ((pin-8U)*4U));
	}
}
