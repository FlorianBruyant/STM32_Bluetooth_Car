#ifndef SSD1306_H_
#define SSD1306_H_

#include <stdint.h>
#include <string.h>
#include "i2c.h"
#include "../CMSIS/stm32f411xe.h"
#include "gpio.h"

#define SSD1306_ADDR  0x3C
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

I2C_Status_t SSD1306_SendCommand(uint8_t addr, uint8_t cmd);

I2C_Status_t SSD1306_SendData(uint8_t addr, uint8_t *buf, uint8_t len);

void SSD1306_Init(void);

void SSD1306_Clear(void);

void SSD1306_WriteChar(uint8_t page, uint8_t col, char c);

void SSD1306_WriteString(uint8_t page, uint8_t col, const char *str);


#endif /* SSD1306_H_ */
