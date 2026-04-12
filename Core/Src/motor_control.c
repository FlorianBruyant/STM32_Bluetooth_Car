/**
 * @file      motor_control.c
 * @brief     Implementation of DC motors controls
 */

#include "../Inc/motor_control.h"
#include "../../Drivers/Device/Inc/gpio.h"

/**
 *  Private function for converting percentage to CCR value
 *  Clamps input to [0, 100] to prevent overflow.
 */
static uint32_t percent_to_ccr(uint8_t speed)
{
    if (speed >= 100U) return PWM_ARR;
    return ((uint32_t)speed * PWM_ARR) / 100U;
}

void Motor_Init(void)
{
    // Enable clocks for GPIOA and B and TIM3
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Configure direction pins
    GPIO_Init(GPIOA, IN1_PIN, GPIO_MODE_OUTPUT, GPIO_OTYPE_PUSHPULL,
              GPIO_SPEED_LOW, GPIO_PULL_NONE);
    GPIO_Init(GPIOA, IN2_PIN, GPIO_MODE_OUTPUT, GPIO_OTYPE_PUSHPULL,
              GPIO_SPEED_LOW, GPIO_PULL_NONE);
    GPIO_Init(GPIOA, IN3_PIN, GPIO_MODE_OUTPUT, GPIO_OTYPE_PUSHPULL,
              GPIO_SPEED_LOW, GPIO_PULL_NONE);
    GPIO_Init(GPIOB, IN4_PIN, GPIO_MODE_OUTPUT, GPIO_OTYPE_PUSHPULL,
              GPIO_SPEED_LOW, GPIO_PULL_NONE);

    // Configure pins connected to ENA and ENB to be used for PWM
    GPIO_Init(GPIOB, ENA_PIN, GPIO_MODE_ALTERNATE, GPIO_OTYPE_PUSHPULL,
              GPIO_SPEED_HIGH, GPIO_PULL_NONE);
    GPIO_SelectAltFunc(GPIOB, ENA_PIN, GPIO_AF_TIM3);

    GPIO_Init(GPIOB, ENB_PIN, GPIO_MODE_ALTERNATE, GPIO_OTYPE_PUSHPULL,
              GPIO_SPEED_HIGH, GPIO_PULL_NONE);
    GPIO_SelectAltFunc(GPIOB, ENB_PIN, GPIO_AF_TIM3);

    /* Configure TIM3 for PWM */

    // Set prescaler and auto reload with values defined in .h
    TIM3->PSC = PWM_PSC;
    TIM3->ARR = PWM_ARR;

    // Configure CH1 and CH2 in PWM mode 1
    TIM3->CCMR1 = 0U;   // Reset before writing
    TIM3->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos)   // CH1 PWM mode 1
                |  TIM_CCMR1_OC1PE              // CH1 preload
                |  (6U << TIM_CCMR1_OC2M_Pos)   // CH2 PWM mode 1
                |  TIM_CCMR1_OC2PE;             // CH2 preload

    /* Enable channel outputs
     * CCER register enables the output on the physical pin :
     *   CC1E : CH1 output enable : activates PB4
     *   CC2E : CH2 output enable : activates PB5
     */
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;

    /* 4d. Initialize CCR to 0 (motors stopped) */
    TIM3->CCR1 = 0U;
    TIM3->CCR2 = 0U;

    // Generate an Update Event to load PSC and ARR immediately
    TIM3->EGR |= TIM_EGR_UG;

    // Start the timer
    TIM3->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;
}

void Motor_SetSpeedLeft(uint8_t speed)
{
    TIM3->CCR1 = percent_to_ccr(speed);
}

void Motor_SetSpeedRight(uint8_t speed)
{
    TIM3->CCR2 = percent_to_ccr(speed);
}

void Motor_SetSpeed(uint8_t speed)
{
    uint32_t ccr = percent_to_ccr(speed);
    TIM3->CCR1 = ccr;
    TIM3->CCR2 = ccr;
}
