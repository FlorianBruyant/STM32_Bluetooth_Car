/**
 * @file    timer.h
 * @brief   SysTick-based time base (1 ms resolution)
 *
 * The SysTick peripheral is configured to fire every millisecond.
 * Each interrupt increments the internal counter ms_tick.
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/**
 * @brief  Initialize the SysTick peripheral for a 1 ms time base.
 *
 *         Assumes SystemCoreClock is set correctly before the call.
 *         Sets SysTick interrupt priority to 0 (highest).
 *         Must be called once at startup, before any other timer function.
 */
void timer_Init(void);

/**
 * @brief  Return the current tick counter value (milliseconds since startup).
 *
 * @return Tick count in milliseconds (wraps around after ~49 days).
 */
uint32_t timer_GetTick(void);

/**
 * @brief  Blocking delay.
 *
 * @param  ms  Number of milliseconds to wait.
 */
void timer_DelayMs(uint32_t ms);

/**
 * @brief  Return the number of milliseconds elapsed since @p start_tick.
 *
 *         Handles the 32-bit wrap-around correctly.
 *
 * @param  start_tick  Reference tick value (obtained from timer_GetTick()).
 * @return Elapsed time in milliseconds.
 */
uint32_t timer_Elapsed(uint32_t start_tick);

#endif /* TIMER_H */
