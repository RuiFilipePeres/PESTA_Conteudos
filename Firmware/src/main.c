#include "stm32f4xx.h"
#include "adc_config.h"
#include "DMA_config.h"
#include "GPIO_init.h"
#include "USART_config.h"
#include "TIM_config.h"
#include <stdio.h>

void SystemClock_Config(void);
void Configure_Flash_WaitStates(void);
void init_function(void);

void TIM1_UP_TIM10_IRQHandler(void) {
    if (TIM1->SR & TIM_SR_UIF) {
        TIM1->SR &= ~TIM_SR_UIF;
        GPIOA->ODR ^= (1 << 6);  // Toggle PA6 (para debug no osciloscópio)
    }
}


int main(void) {
    //3 wait cycles -> 100MHz
    Configure_Flash_WaitStates();

    // Clock 100MHz
    SystemClock_Config();

    // Delay de inicialização
    for (int i = 0; i < 1000000; i++);

    // Atualiza o SystemCoreClock
    SystemCoreClockUpdate();
    for (int i = 0; i < 1000000; i++);
    init_function();

    // Loop principal
    while (1) {

    }

}

void init_function (void) {
    GPIO_init();
    TIM11_init();
    TIM1_CH1_trigger_init(48000);  // 48 kHz PWM em PA8
    USART1_init();
    DMA2_Stream0_init();
    ADC1_init();
}

// Configuração do Clock do Sistema para 100 MHz (usando o HSE de 8 MHz)
void SystemClock_Config(void) {
    // Habilita o HSE (High-Speed External)
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));  // Espera o HSE estabilizar

    // Configura o PLL para gerar 100 MHz (a partir de HSE = 8 MHz)
    RCC->PLLCFGR = (4 << RCC_PLLCFGR_PLLM_Pos)  // PLLM = 4 (8 MHz / 4 = 2 MHz)
                 | (100 << RCC_PLLCFGR_PLLN_Pos) // PLLN = 100 (2 MHz × 100 = 200 MHz)
                 | (0 << RCC_PLLCFGR_PLLP_Pos)   // PLLP = 2 (200 MHz / 2 = 100 MHz)
                 | RCC_PLLCFGR_PLLSRC_HSE;       // Fonte do PLL = HSE

    // Habilita o PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));  // Espera o PLL estabilizar

    // Configura os prescalers do barramento
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB Prescaler = 1 (mantém 100 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 Prescaler = 2 (máx. 50 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 Prescaler = 1 (máx. 100 MHz)

    // Seleciona o PLL como clock principal
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // Atualiza a variável SystemCoreClock
    SystemCoreClockUpdate();
}

// Número de wait states da Flash
void Configure_Flash_WaitStates(void) {
    // Registo FLASH->ACR 3 ciclos Wait states
    FLASH->ACR &= ~FLASH_ACR_LATENCY;  // Limpa os bits de latência
    FLASH->ACR |= FLASH_ACR_LATENCY_3WS;  // Configura para 3 wait states
}



