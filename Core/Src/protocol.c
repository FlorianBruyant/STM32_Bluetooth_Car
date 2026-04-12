/*
 * @file protocol.c
 * @brief contains command parser and rx process functions for UART
 */

#include "protocol.h"

static uint8_t parse_and_execute(uint8_t dir, uint8_t d1, uint8_t d2)
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
