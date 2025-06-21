/*
 * TIM_Config.c
 *
 *  Created on: 05/04/2025
 *      Author: 35191
 */

#include "TIM_config.h"

// LED a 1Hz
void TIM11_init(void) {
    // Habilita Clock TIM11
    RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;

    // PREScaler e AutoReload
    TIM11->PSC = 10000 - 1;         // Prescaler: reduz a frequência
    TIM11->ARR = 10000 - 1;         // Auto-reload: 1 Hz

    // Interrupção de atualização
    TIM11->DIER |= TIM_DIER_UIE;
    TIM11->SR &= ~TIM_SR_UIF;

    // Habilita TIM11
    TIM11->CR1 |= TIM_CR1_CEN;

    // Configura o NVIC para TIM11 (compartilhado com TIM1 TRG/COM)
    NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 1);
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
}

// Handler para TIM11 (e TIM1 TRG/COM)
void TIM1_TRG_COM_TIM11_IRQHandler(void) {
    if (TIM11->SR & TIM_SR_UIF) {
        TIM11->SR &= ~TIM_SR_UIF;
        GPIOA->ODR ^= (1 << 5);  // Toggle LED em PA5
    }
}

// PWM TIM1_CH1 Trigger do ADC (EXTSEL = 0000)
void TIM1_CH1_trigger_init(uint32_t sample_rate_hz) {
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    uint32_t timer_clk = SystemCoreClock;
    uint32_t arr = (timer_clk / sample_rate_hz) - 1;

    TIM1->PSC = 0;
    TIM1->ARR = arr;
    TIM1->CCR1 = (arr + 1) / 2;  // 50% duty cycle

    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);  // PWM mode 1
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;            // Preload enable

    TIM1->CCER |= TIM_CCER_CC1E;

    TIM1->EGR |= TIM_EGR_UG;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;
}




