/**
 * @file    i2c.h
 * @brief   I2C1 driver - PB8 (SCL) / PB9 (SDA), bare metal CMSIS
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"

/**
 *  Return codes
 */
typedef enum {
    I2C_OK = 0U, 	 // Transaction successful
    I2C_ERR_AF = 1U, // Acknowledge Failure (no device)
    I2C_ERR_TO = 2U, // Timeout waiting for flag
    I2C_ERR_BUS = 3U // Bus busy or arbitration lost
} I2C_Status_t;

/**
 * @brief  Initialize I2C2 on PB3 (SDA) and PB10 (SCL).
 *         Standard mode, 100 kHz, 16 MHz APB1 clock.
 */
void I2C_Init(void);

/**
 * @brief  Write one or more bytes to a device register.
 * @param  dev_addr  	7-bit device address (not shifted)
 * @param  reg_addr 	Register address (1 byte)
 * @param  data			Pointer to data buffer
 * @param  len			Number of bytes to write
 * @return I2C_OK on success, error code otherwise
 */
I2C_Status_t I2C_Write(uint8_t dev_addr, uint8_t reg_addr,
		uint8_t *data, uint8_t len);

/**
 * @brief  Read one or more bytes from a device register.
 * @param  dev_addr 	7-bit device address (not shifted)
 * @param  reg_addr   	Register address (1 byte)
 * @param  data       	Pointer to destination buffer
 * @param  len        	Number of bytes to read
 * @return I2C_OK on success, error code otherwise
 */
I2C_Status_t I2C_Read(uint8_t dev_addr, uint8_t reg_addr,
		uint8_t *data, uint8_t len);

#endif /* I2C_H */
