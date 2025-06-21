#include "adc_config.h"

#define VREF 3.3f

void ADC1_init(void) {
    // 1. Ativa Clock do ADC1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 2. Modo SCAN (múltiplos canais)
    ADC1->CR1 |= ADC_CR1_SCAN;

    // 3. Trigger externo TIM1_TRGO
    ADC1->CR2 &= ~ADC_CR2_EXTSEL;      				// Limpa EXTSEL[3:0] (bits 24-27)
    ADC1->CR2 |= (0b0000 << ADC_CR2_EXTSEL_Pos);    //  Trigger externo = TIM1_CC1 -> EXTSEL = 0000
    ADC1->CR2 |= ADC_CR2_EXTEN_0;                   // Trigger na borda de subida (EXTEN = 01)

    // 4. Configura o número de conversões (2 canais)
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= (1 << ADC_SQR1_L_Pos);  			// 2 conversões -> L = N-1 = 1

    // 5. Sequência dos canais
    ADC1->SQR3 = 0;
    ADC1->SQR3 |= (0 << (0 * 5));  					// Canal 0 -> PA0
    ADC1->SQR3 |= (1 << (1 * 5));  					// Canal 1 -> PA1

    // 6. Resolução e alinhamento
    ADC1->CR1 &= ~ADC_CR1_RES;        				// 12 bits
    ADC1->CR2 &= ~ADC_CR2_ALIGN;       				// Alinhado à direita

    // 7. DMA + DDS (continuação automática)
    ADC1->CR2 |= ADC_CR2_DMA;
    ADC1->CR2 |= ADC_CR2_DDS;

    // 8. Ativa ADC
    ADC1->CR2 |= ADC_CR2_ADON;

    // 9. Pequeno delay
    for (volatile int i = 0; i < 1000; i++);
}

uint16_t ADC1_read(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC));
    uint16_t value = ADC1->DR;
    ADC1->SR &= ~ADC_SR_EOC;
    return value;
}
