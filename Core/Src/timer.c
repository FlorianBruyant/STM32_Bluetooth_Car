/**
 * @file    timer.c
 * @brief   SysTick-based time base (1 ms resolution)
 *
 * The SysTick peripheral is configured to fire every millisecond.
 * Each interrupt increments the internal counter ms_tick.
 */

#include "timer.h"
#include "stm32f411xe.h"

static volatile uint32_t ms_tick = 0U;

extern uint32_t SystemCoreClock;

void SysTick_Handler(void)
{
    ms_tick++;
}

void timer_Init(void)
{
    SysTick_Config(SystemCoreClock / 1000U);
    NVIC_SetPriority(SysTick_IRQn, 0U);
}

uint32_t timer_GetTick(void)
{
    return ms_tick;
}

void timer_DelayMs(uint32_t ms)
{
    uint32_t start = timer_GetTick();
    while (timer_Elapsed(start) < ms) {}
}

uint32_t timer_Elapsed(uint32_t start_tick)
{
    /* Subtraction is wrap-around safe for unsigned 32-bit arithmetic */
    return (timer_GetTick() - start_tick);
}


