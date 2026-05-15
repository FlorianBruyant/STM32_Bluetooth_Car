/**
 * @file    i2c.c
 * @brief   I2C1 driver : bare metal CMSIS, STM32F411RE
 * Uses PB8 (SCL) and PB9 (SDA) with AF4
 */

#include "i2c.h"
#include "gpio.h"
#include "stm32f411xe.h"

/**
 *  Constants
 */

/* Timeout loop counter, prevents infinite wait on bus errors */
#define I2C_TIMEOUT 100000U

/* CCR value for 100 kHz standard mode at 16 MHz APB1 */
#define I2C_CCR_VAL 80U

/* TRISE value for 100 kHz standard mode at 16 MHz APB1 */
#define I2C_TRISE_VAL 17U

/* APB1 frequency in MHz (used in CR2) */
#define I2C_PCLK_MHZ  16U

static I2C_Status_t wait_flag_sr1(uint32_t flag)
{
    uint32_t timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & flag))
    {
        // Check for acknowledge failure
        if (I2C1->SR1 & I2C_SR1_AF)
        {
            I2C1->SR1 &= ~I2C_SR1_AF; // Clear AF flag
            I2C1->CR1 |= I2C_CR1_STOP; // Generate STOP
            return I2C_ERR_AF;
        }

        if (--timeout == 0U)
        	return I2C_ERR_TO;
    }
    return I2C_OK;
}

void I2C_Init(void)
{
    // Step 1: Enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    __NOP();
	__NOP();

    // Step 3: Full reset of I2C1 via RCC
    RCC->APB1RSTR |=  RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    // Step 4: Configure GPIO pins as open-drain BEFORE I2C1 touches them
    // PB8 = SCL, PB9 = SDA (I2C1) with AF4
    // PB8 = SCL, PB9 = SDA (I2C1) with AF4
    GPIO_Init(GPIOB, 8U, GPIO_MODE_ALTERNATE, GPIO_OTYPE_OPENDRAIN,
              GPIO_SPEED_HIGH, GPIO_PULL_UP);
    GPIO_SelectAltFunc(GPIOB, 8U, 4U);  // AF4 for I2C1 SCL

    GPIO_Init(GPIOB, 9U, GPIO_MODE_ALTERNATE, GPIO_OTYPE_OPENDRAIN,
              GPIO_SPEED_HIGH, GPIO_PULL_UP);
    GPIO_SelectAltFunc(GPIOB, 9U, 4U);  // AF4 for I2C1 SDA

    // PE must be 0 during configuration
    I2C1->CR1 &= ~I2C_CR1_PE;
    // Step 5: Configure I2C1 clock and timing
    I2C1->CR2 = (I2C_PCLK_MHZ & I2C_CR2_FREQ);  // Changed: I2C1 instead of I2C2
    I2C1->CCR = I2C_CCR_VAL;
    I2C1->TRISE = I2C_TRISE_VAL;

    // Step 6: Enable I2C1
    I2C1->CR1 |= I2C_CR1_PE;
}

I2C_Status_t I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
    I2C_Status_t status;

    /* Wait for bus to be free */
    uint32_t bus_timeout = 100000;
    while ((I2C1->SR2 & I2C_SR2_BUSY) && --bus_timeout);
    if (bus_timeout == 0) return I2C_ERR_BUS;

    /* Generate START condition */
    I2C1->CR1 |= I2C_CR1_START;
    status = wait_flag_sr1(I2C_SR1_SB);    /* Wait SB : START sent */
    if (status != I2C_OK) return status;

    /* Send device address + Write bit (addr << 1 | 0) */
    I2C1->DR = (uint8_t)((dev_addr << 1U) & 0xFEU);
    status = wait_flag_sr1(I2C_SR1_ADDR);  /* Wait ADDR : address sent + ACK */
    if (status != I2C_OK) return status;
    (void)I2C1->SR2;                        /* Clear ADDR by reading SR1+SR2  */

    /* Send register address */
    I2C1->DR = reg_addr;
    status = wait_flag_sr1(I2C_SR1_TXE);   /* Wait TXE : DR empty */
    if (status != I2C_OK) return status;

    /* Send data bytes */
    for (uint8_t i = 0U; i < len; i++)
    {
        I2C1->DR = data[i];
        status = wait_flag_sr1(I2C_SR1_TXE);
        if (status != I2C_OK) return status;
    }

    /* Wait for BTF (Byte Transfer Finished) before STOP
     * Ensures the last byte has been fully shifted out before
     * generating STOP — without this, STOP can corrupt the last byte */
    status = wait_flag_sr1(I2C_SR1_BTF);
    if (status != I2C_OK) return status;

    /* Generate STOP condition */
    I2C1->CR1 |= I2C_CR1_STOP;

    return I2C_OK;
}

I2C_Status_t I2C_Read(uint8_t dev_addr, uint8_t reg_addr,
                       uint8_t *data, uint8_t len)
{
    I2C_Status_t status;

    /* Wait for bus to be free */
	uint32_t timeout = I2C_TIMEOUT;
	while ((I2C1->SR2 & I2C_SR2_BUSY) && --timeout);
	if (timeout == 0U) return I2C_ERR_BUS;

    /* Phase 1 : Write register address */
    I2C1->CR1 |= I2C_CR1_START;
    status = wait_flag_sr1(I2C_SR1_SB);
    if (status != I2C_OK) return status;

    I2C1->DR = (uint8_t)((dev_addr << 1U) & 0xFEU);  /* addr + Write */
    status = wait_flag_sr1(I2C_SR1_ADDR);
    if (status != I2C_OK) return status;
    (void)I2C1->SR2;

    I2C1->DR = reg_addr;
    status = wait_flag_sr1(I2C_SR1_BTF);
    if (status != I2C_OK) return status;

    /* Phase 2 : Repeated START + read */

    /* Enable ACK before restart so the first received bytes are ACKed */
    I2C1->CR1 |= I2C_CR1_ACK;

    I2C1->CR1 |= I2C_CR1_START;
    status = wait_flag_sr1(I2C_SR1_SB);
    if (status != I2C_OK) return status;

    I2C1->DR = (uint8_t)((dev_addr << 1U) | 0x01U);  /* addr + Read */
    status = wait_flag_sr1(I2C_SR1_ADDR);
    if (status != I2C_OK) return status;

    if (len == 1U)
    {
        /* Special case : single byte read
         * Disable ACK and set STOP BEFORE clearing ADDR
         * so the hardware is ready to NACK+STOP immediately */
        I2C1->CR1 &= ~I2C_CR1_ACK;
        (void)I2C1->SR2;                    /* Clear ADDR */
        I2C1->CR1 |= I2C_CR1_STOP;
        status = wait_flag_sr1(I2C_SR1_RXNE);
        if (status != I2C_OK) return status;
        data[0] = (uint8_t)I2C1->DR;
    }
    else
    {
        /* Multi-byte read */
        (void)I2C1->SR2; /* Clear ADDR */

        for (uint8_t i = 0U; i < len; i++)
        {
            if (i == (len - 1U))
            {
                /* Before reading last byte :
                 * disable ACK and generate STOP */
                I2C1->CR1 &= ~I2C_CR1_ACK;
                I2C1->CR1 |= I2C_CR1_STOP;
            }
            status = wait_flag_sr1(I2C_SR1_RXNE);
            if (status != I2C_OK) return status;
            data[i] = (uint8_t)I2C1->DR;
        }
    }

    return I2C_OK;
}
