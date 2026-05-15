#include "VL53L0X_platform.h"
#include "i2c.h"
#include "timer.h"

VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev,
                                  uint8_t index,
                                  uint8_t *pdata,
                                  uint32_t count)
{
    I2C_Status_t s = I2C_Write(Dev->I2cDevAddr,
                                index, pdata, (uint8_t)count);
    return (s == I2C_OK) ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev,
                                 uint8_t index,
                                 uint8_t *pdata,
                                 uint32_t count)
{
    I2C_Status_t s = I2C_Read(Dev->I2cDevAddr,
                               index, pdata, (uint8_t)count);
    return (s == I2C_OK) ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev,
                               uint8_t index, uint8_t data)
{
    return VL53L0X_WriteMulti(Dev, index, &data, 1U);
}

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev,
                               uint8_t index, uint8_t *data)
{
    return VL53L0X_ReadMulti(Dev, index, data, 1U);
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev,
                               uint8_t index, uint16_t data)
{
    uint8_t buf[2] = { (uint8_t)(data >> 8U), (uint8_t)(data & 0xFFU) };
    return VL53L0X_WriteMulti(Dev, index, buf, 2U);
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev,
                               uint8_t index, uint16_t *data)
{
    uint8_t buf[2];
    VL53L0X_Error e = VL53L0X_ReadMulti(Dev, index, buf, 2U);
    if (e == VL53L0X_ERROR_NONE)
        *data = (uint16_t)((buf[0] << 8U) | buf[1]);
    return e;
}

VL53L0X_Error VL53L0X_GetTickCount(uint32_t *ptick)
{
    *ptick = timer_GetTick();
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_WaitUs(VL53L0X_DEV Dev, int32_t wait_us)
{
    /* Use timer-based delay for all delays >= 1000us
     * For delays < 1000us, round up to 1ms to avoid compiler
     * optimization removing busy-wait loops, and timer has 1ms resolution anyway */
    if (wait_us <= 0)
        return VL53L0X_ERROR_NONE;
    
    uint32_t ms = (wait_us + 999U) / 1000U;
    timer_DelayMs(ms);
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data) {
    uint8_t buf[4];
    buf[0] = (uint8_t)(data >> 24U);
    buf[1] = (uint8_t)((data >> 16U) & 0xFFU);
    buf[2] = (uint8_t)((data >> 8U)  & 0xFFU);
    buf[3] = (uint8_t)(data & 0xFFU);
    return VL53L0X_WriteMulti(Dev, index, buf, 4U);
}

VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data) {
    uint8_t buf[4];
    VL53L0X_Error e = VL53L0X_ReadMulti(Dev, index, buf, 4U);
    if (e == VL53L0X_ERROR_NONE) {
        *data = ((uint32_t)buf[0] << 24U) |
                ((uint32_t)buf[1] << 16U) |
                ((uint32_t)buf[2] << 8U)  |
                ((uint32_t)buf[3]);
    }
    return e;
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData) {
    uint8_t data;
    VL53L0X_Error status = VL53L0X_RdByte(Dev, index, &data);
    if (status == VL53L0X_ERROR_NONE) {
        data = (data & AndData) | OrData;
        status = VL53L0X_WrByte(Dev, index, data);
    }
    return status;
}
VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev) {
    timer_DelayMs(5U);
    return VL53L0X_ERROR_NONE;
}
