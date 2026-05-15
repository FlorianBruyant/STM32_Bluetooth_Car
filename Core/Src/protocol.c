/*
 * @file protocol.c
 * @brief contains command parser and rx process functions for UART,
 * and init and obstacle checking for tof sensor
 */

#include "protocol.h"

extern volatile uint8_t obstacle_front;

// Global variables for TOF
VL53L0X_Dev_t tof_device;
VL53L0X_DEV Dev = &tof_device;

uint8_t parse_and_execute(uint8_t dir, uint8_t d1, uint8_t d2)
{
    // Validate direction byte
    if (dir != 'F' && dir != 'B' &&
        dir != 'L' && dir != 'R' && dir != 'S')
    {
        return 0U;
    }

    // Validate speed bytes : must be ASCII digits '0'-'9'
    if (d1 < '0' || d1 > '9' || d2 < '0' || d2 > '9')
    {
        return 0U;
    }

    // Convert ASCII digits to integer speed value
    uint8_t speed = (uint8_t)(((d1 - '0') * 10U) + (d2 - '0'));

    // Clamp speed to valid range
    if (speed > 99U) speed = 99U;

    // Check if an obstacle is in front
	if ((dir == 'F') && (obstacle_front == 1U))
	{
		return 0U;
	}

    // Execute command
    switch (dir)
    {
        case 'F':
        	motor_Forward(speed);
        	break;
        case 'B':
        	motor_Backward(speed);
        	break;
        case 'L':
        	motor_TurnLeft(speed);
        	break;
        case 'R':
        	motor_TurnRight(speed);
        	break;
        case 'S':
        	motor_Stop();
        	break;
        default:
        	return 0U;
    }
    return 1U;
}

void protocol_ProcessRx(uint32_t *last_cmd_tick)
{
    static ParserState_t state = STATE_WAIT_DIR;
    static uint8_t frame_dir  = 0U;
    static uint8_t frame_d1   = 0U;

    uint8_t byte;

    while (UART_Read(&byte))
    {

        switch (state)
        {
            case STATE_WAIT_DIR:
                /* Accept known direction bytes only */
                if (byte == 'F' || byte == 'B' ||
                    byte == 'L' || byte == 'R' || byte == 'S')
                {
                    frame_dir = byte;
                    state = STATE_WAIT_D1;
                }
                /* Any other byte is ignored */
                break;

            case STATE_WAIT_D1:
                if (byte >= '0' && byte <= '9')
                {
                    frame_d1 = byte;
                    state = STATE_WAIT_D2;
                }
                else
                {
                    /* Unexpected byte : reset and try this byte as direction */
                    state = STATE_WAIT_DIR;
                    if (byte == 'F' || byte == 'B' ||
                        byte == 'L' || byte == 'R' || byte == 'S')
                    {
                        frame_dir = byte;
                        state = STATE_WAIT_D1;
                    }
                }
                break;

            case STATE_WAIT_D2:
                if (byte >= '0' && byte <= '9')
                {
                    /* Complete frame received - execute */
                    if (parse_and_execute(frame_dir, frame_d1, byte))
                    {
                        *last_cmd_tick = timer_GetTick(); // Reset watchdog
                        UART_SendString("OK\r\n"); // Acknowledge
                    }
                }
                /* Reset state - start looking for next frame */
                state = STATE_WAIT_DIR;
                break;

            default:
                state = STATE_WAIT_DIR;
                break;
        }
    }
}

/**
 * VL53L0X Distance Sensor - Full initialization and test
 *
 * Correct initialization sequence (per ST UM2039 / API examples):
 *   1. DataInit
 *   2. StaticInit
 *   3. PerformRefSpadManagement
 *   4. PerformRefCalibration
 *   5. SetDeviceMode(CONTINUOUS)
 *   6. StartMeasurement
 */

void tof_init_protocol(void)
{
    char dbg[100];
    int32_t status;

    UART_SendString("\r\n=== VL53L0X DISTANCE SENSOR INIT ===\r\n");

    /* Step 1 : Data init */
    UART_SendString("Step 1: DataInit...\r\n");
    Dev->I2cDevAddr = 0x29U;

    status = VL53L0X_DataInit(Dev);
    sprintf(dbg, "VL53L0X_DataInit() = %ld\r\n", status);
    UART_SendString(dbg);
    if (status != VL53L0X_ERROR_NONE) { while(1); }
    UART_SendString("DataInit OK\r\n");

    /* Step 2 : static init */
    UART_SendString("Step 2: StaticInit...\r\n");
    status = VL53L0X_StaticInit(Dev);
    sprintf(dbg, "VL53L0X_StaticInit() = %ld\r\n", status);
    UART_SendString(dbg);
    if (status != VL53L0X_ERROR_NONE) { while(1); }
    UART_SendString("StaticInit OK\r\n");

    /* Step 3 : SPAD calibration */
    UART_SendString("Step 3: SPAD calibration (PerformRefSpadManagement)...\r\n");
    uint32_t refSpadCount    = 0U;
    uint8_t  isApertureSpads = 0U;
    status = VL53L0X_PerformRefSpadManagement(Dev, &refSpadCount, &isApertureSpads);
    sprintf(dbg, "PerformRefSpadManagement() = %ld  (SPADs=%lu, aperture=%d)\r\n",
            status, refSpadCount, (int)isApertureSpads);
    UART_SendString(dbg);
    if (status != VL53L0X_ERROR_NONE)
        UART_SendString("WARNING: SPAD management failed, continuing anyway\r\n");
    else
        UART_SendString("SPAD calibration OK\r\n");

    /* Step 4 : Reference calibration (VHV + PhaseCal) */
    UART_SendString("Step 6: Ref calibration (PerformRefCalibration)...\r\n");
    uint8_t  vhvSettings = 0;
    uint8_t phaseCal = 0U;
    status = VL53L0X_PerformRefCalibration(Dev, &vhvSettings, &phaseCal);
    sprintf(dbg, "PerformRefCalibration() = %ld  (VHV=%d, PhaseCal=%d)\r\n",
            status, (int)vhvSettings, (int)phaseCal);
    UART_SendString(dbg);
    if (status != VL53L0X_ERROR_NONE)
        UART_SendString("WARNING: Ref calibration failed, continuing anyway\r\n");
    else
        UART_SendString("Ref calibration OK\r\n");

    /* Step 5 : Set CONTINUOUS mode */
    UART_SendString("Step 7: Setting device mode to CONTINUOUS RANGING...\r\n");
    status = VL53L0X_SetDeviceMode(Dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    sprintf(dbg, "VL53L0X_SetDeviceMode() = %ld\r\n", status);
    UART_SendString(dbg);
    if (status != VL53L0X_ERROR_NONE) { while(1); }
    UART_SendString("Mode set to CONTINUOUS_RANGING\r\n");

    /* Step 6 : Start measurement */
    UART_SendString("Step 8: Starting continuous measurement...\r\n");
    status = VL53L0X_StartMeasurement(Dev);
    sprintf(dbg, "VL53L0X_StartMeasurement() = %ld\r\n", status);
    UART_SendString(dbg);
    if (status != VL53L0X_ERROR_NONE) { while(1); }
    UART_SendString("Measurement started\r\n");

    /* Step 7 : Read distances
     *
     * RangeStatus meanings:
     *   0 = OK
     *   1 = Sigma fail    (too much noise)
     *   2 = Signal fail   (signal too weak)
     *   3 = Min range     (target too close < ~30 mm)
     *   4 = Phase fail    (calibration issue / no target)
     *   5 = Hardware fail
     */
    UART_SendString("Step 9: Reading distances...\r\n");
    UART_SendString("(place an object 10-200 cm in front of the sensor)\r\n");
    timer_DelayMs(100U);

    for (int i = 0; i < 30; i++)
    {
        uint8_t dataReady = 0U;
        status = VL53L0X_GetMeasurementDataReady(Dev, &dataReady);

        if (status != VL53L0X_ERROR_NONE)
        {
            sprintf(dbg, "[%d] GetMeasurementDataReady error: %ld\r\n", i, status);
            UART_SendString(dbg);
            timer_DelayMs(100U);
            continue;
        }

        if (dataReady)
        {
            VL53L0X_RangingMeasurementData_t rangingData;
            status = VL53L0X_GetRangingMeasurementData(Dev, &rangingData);

            if (status != VL53L0X_ERROR_NONE)
            {
                sprintf(dbg, "[%d] GetRangingMeasurementData error: %ld\r\n", i, status);
                UART_SendString(dbg);
            }
            else
            {
                uint16_t distance = rangingData.RangeMilliMeter;
                uint8_t  range_status = rangingData.RangeStatus;

                /* Clear AFTER reading so the sensor triggers the next shot */
                VL53L0X_ClearInterruptMask(Dev, 0);

                sprintf(dbg, "[%d] Dist=%dmm, Stat=%d", i, (int)distance, (int)range_status);
                UART_SendString(dbg);

                if (range_status == 0U && distance >= 30U && distance <= 2000U)
                    UART_SendString("  -> VALID\r\n");
                else if (distance == 8191U || distance == 8190U)
                    UART_SendString("  -> OUT OF RANGE\r\n");
                else
                {
                    switch (range_status)
                    {
                        case 1U: UART_SendString("  -> sigma fail\r\n");    break;
                        case 2U: UART_SendString("  -> signal fail\r\n");   break;
                        case 3U: UART_SendString("  -> target too close\r\n"); break;
                        case 4U: UART_SendString("  -> phase fail\r\n");    break;
                        case 5U: UART_SendString("  -> hardware fail\r\n"); break;
                        default: UART_SendString("\r\n");                   break;
                    }
                }
            }
        }
        else
        {
            sprintf(dbg, "[%d] dataReady=0 (waiting...)\r\n", i);
            UART_SendString(dbg);
        }

        timer_DelayMs(100U);
    }

    UART_SendString("\r\nStep 9: Test complete\r\n");
    UART_SendString("========== VL53L0X TEST END ==========\r\n");
}


/**
 * Obstacle verification - called from main loop
 *
 * Uses the same read-then-clear pattern as ToF_Test.
 * The sensor must have been initialized before the main loop starts.
 */
void check_obstacle(void)
{
    uint8_t dataReady = 0U;
    VL53L0X_GetMeasurementDataReady(Dev, &dataReady);

    if (dataReady == 1U)
    {
        VL53L0X_RangingMeasurementData_t rangingData;
        VL53L0X_GetRangingMeasurementData(Dev, &rangingData);

        /* Clear AFTER reading - triggers next continuous acquisition */
        VL53L0X_ClearInterruptMask(Dev, 0);

        uint16_t distance = rangingData.RangeMilliMeter;
        uint8_t  status   = rangingData.RangeStatus;

        /*char dbg[48];
        sprintf(dbg, "Dist: %d mm, Stat: %d\r\n", (int)distance, (int)status);
        UART_SendString(dbg);*/

        /* Only act on valid readings (status 0 or 1, within sensor range) */
        if ((status == 0U || status == 1U) && distance > 20U && distance < 2000U)
        {
            if (distance < BRAKE_DISTANCE_MM)
            {
                if (obstacle_front == 0U)
                {
                    obstacle_front = 1U;
                    motor_Stop();
                    UART_SendString("OBSTACLE IN FRONT\r\n");
                }
            }
            else
            {
                if (obstacle_front == 1U)
                {
                    obstacle_front = 0U;
                    UART_SendString("NO OBSTACLE\r\n");
                }
            }
        }
    }
}
